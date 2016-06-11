#include "nt.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "avrutil.h"

#define BUF_SIZE_MASK (NTBUS_BUFSIZE-1)

NtRingBuf::NtRingBuf() : wrtInd(0), unread(0)
{

}

bool NtRingBuf::pop(uint8_t* out)
{
	if (unread > 0)
	{
		*out = slots[(wrtInd - unread) & BUF_SIZE_MASK];
		unread--;
		return true;
	}
	else
	{
		return false;
	}
}

void NtRingBuf::push(uint8_t in)
{
	slots[wrtInd++ & BUF_SIZE_MASK] = in;
	unread++;
}

// module wide buffer storing incoming serial
static NtRingBuf ntBuffer = NtRingBuf();


NtNode::NtNode(uint8_t id, NtRingBuf* buffer) :
		matchIdGetData(NTBUS_STX | NTBUS_GET | id),
		buffer(buffer),
		busState(IDLE)
{
	// setting UART to 2MBaud
	UCSR0A |= (1 << U2X0);
	UBRR0H = 0;
	UBRR0L = 1;

	// ignore bytes with leftmost bit == 1
	// TODO

	// set 8 bits, 1 stop, no parity
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

	// enable RX line
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0);
}

NtNode NtNode::createNtNode(uint8_t id)
{
	NtNode ntNode = NtNode(id, &ntBuffer);
	return ntNode;
}

bool NtNode::processBusData(uint8_t* recv)
{
	const bool ret = buffer->pop(recv);
	if (ret)
	{
		switch (busState)
		{
		case IDLE:
			if (*recv == (NTBUS_STX | NTBUS_TRIGGER))
			{
				busState = TRIGGERED;
			}
			break;
		case TRIGGERED:
			if (*recv == matchIdGetData)
			{
				busState = GETDATA;
			}
			else if (*recv == (NTBUS_STX | NTBUS_SET | NTBUS_ID_MOTORALL))
			{
				busState = MOTORDATA;
			}
			break;
		case GETDATA:
			// specific NtNode types may have its own actions.
			if (*recv == NTBUS_CMD_GETVERSIONSTR)
			{
				tNTBusCmdGetVersionStrData vStrData;
				memset(vStrData.VersionStr, 0, NTBUS_CMDGETVERSIONSTR_DATALEN);
				strcpy(vStrData.VersionStr, STORM32NTBUS_VERSIONSTR);

				UCSR0B |= (1<<TXEN0);    // enable TX line
				writeFrame((uint8_t*) vStrData.VersionStr, NTBUS_CMDGETVERSIONSTR_DATALEN);
				UCSR0B &= (1<<TXEN0);    // disable TX line

			}
			break;
		case MOTORDATA:
			// is the responsibility of the specific NtNode to act on this.
			mtrDatChars++;
			if (mtrDatChars >= 10)
			{
				busState = IDLE;
			}

			break;
		}
	}
	return ret;
}

inline NtNode::NtState NtNode::getBusState() const
{
	return busState;
}


NtNodeImu::NtNodeImu(uint8_t id, NtRingBuf* buffer, tNTBusGetImuData* imudata, uint16_t model) :
		NtNode(id, buffer),
		mImudata(imudata),
		modelCode(model)
{

}


bool NtNodeImu::processBusData(uint8_t* recv)
{
	bool ret = NtNode::processBusData(recv);
	if (getBusState() == GETDATA)
	{
		if (*recv == NTBUS_CMD_GETSTATUS)
		{
			tNTBusCmdGetStatusData statusData;
			statusData.Status = NTBUS_IMU_STATUS_IMU_PRESENT;
			statusData.State = mImudata->ImuStatus;

			UCSR0B |= (1<<TXEN0);    // enable TX line
			writeFrame((uint8_t*) &statusData, NTBUS_CMDGETSTATUS_DATALEN);
			UCSR0B &= (1<<TXEN0);    // disable TX line
		}
		else if (*recv == NTBUS_CMD_GETCONFIGURATION)
		{
			UCSR0B |= (1<<TXEN0);    // enable TX line
			writeFrame((uint8_t*) &modelCode, NTBUS_CMDGETCONFIGURATION_DATALEN);
			UCSR0B &= (1<<TXEN0);    // disable TX line
		}
	}
	return ret;
}

void NtNodeImu::writeImuData() const
{
	uint8_t* c = (uint8_t*) mImudata;
	uint8_t crc = 0;
	for (uint8_t i=0; i<NTBUS_GETIMU_DATALEN; i++)
	{
		usart0_write(*(c++));
		crc ^= *c;
	}
    
	usart0_write(crc);
}


void writeFrame(uint8_t* frame, uint8_t len)
{
	uint8_t crc = 0;
	for (uint8_t i=0; i<len; i++)
	{
		usart0_write(*(frame++));
		crc ^= *frame;
	}

	usart0_write(crc);
}


uint8_t ntcrc(uint8_t* frame, uint8_t length)
{
	uint8_t crc = 0;
	for (uint8_t i=0; i<length; i++)
	{
		crc ^= frame[i];
	}

	return crc;
}


ISR(USART_RX_vect)
{
	// routine is atomic
	// observed externally, wrtInd and unread are incremented simultaneously
	ntBuffer.slots[ntBuffer.wrtInd++ & BUF_SIZE_MASK] = UDR0;
	ntBuffer.unread++;
}
