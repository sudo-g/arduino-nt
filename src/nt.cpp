#include "nt.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "avrutil.h"

#define BUF_SIZE_MASK (NTBUS_BUFSIZE-1)

static void writeFrame(uint8_t* frame, uint8_t len);

NtRingBuf::NtRingBuf() : wrtInd(0), unread(0)
{

}

bool NtRingBuf::pop(uint8_t* out)
{
	// if push() is atomic, 'wrtInd' and 'unread' are incremented together
	// and 'unread' is ONLY incremented by push()
	// which makes this thread safe
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

inline void NtRingBuf::push(uint8_t in)
{
	slots[wrtInd++ & BUF_SIZE_MASK] = in;
    unread = (unread > BUF_SIZE_MASK) ? NTBUS_BUFSIZE : (unread+1);
}

uint8_t NtRingBuf::getUnreadBytes() const
{
	return unread;
}

// module wide buffer storing incoming serial
static NtRingBuf ntBuffer = NtRingBuf();


NtNode::NtNode(uint8_t id, const char* board, NtRingBuf* buffer) :
		matchIdGetData(NTBUS_STX | NTBUS_GET | id),
		matchIdCommand(NTBUS_STX | NTBUS_CMD | id),
		busState(IDLE),
		buffer(buffer)
{
	memcpy(boardStr, board, NTBUS_CMDGETBOARDSTR_DATALEN);

	// setting UART to 2MBaud
	UCSR0A |= (1 << U2X0);
	UBRR0H = 0;
	UBRR0L = 0;

	// ignore bytes with leftmost bit == 1
	// TODO

	// set 8 bits, 1 stop, no parity
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

	// enable RX line
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0);
}

void NtNode::writeBoardStr(char* buf) const
{
	memcpy(buf, boardStr, NTBUS_CMDGETBOARDSTR_DATALEN);
}

NtNode::NtState NtNode::getBusState() const
{
	return busState;
}

int8_t NtNode::processBusData(uint8_t* recv)
{
	const bool hasChar = buffer->pop(recv);
	int8_t rc = -1;
	if (hasChar)
	{
		rc = 0;
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
				rc = 1;    // only specific NtNode types handle this.
			}
			else if (*recv == matchIdCommand)
			{
				busState = COMMANDED;
			}
			break;
		case GETDATA:
			// GETDATA handled by subclass, this block is never reached.
			break;
		case COMMANDED:
			if (*recv == NTBUS_CMD_GETVERSIONSTR)
			{
				tNTBusCmdGetVersionStrData vStrData;
				memset(vStrData.VersionStr, 0, NTBUS_CMDGETVERSIONSTR_DATALEN);
				strcpy(vStrData.VersionStr, STORM32NTBUS_VERSIONSTR);

				WITH_USART0_TX_ENABLED(
					writeFrame((uint8_t*) vStrData.VersionStr, NTBUS_CMDGETVERSIONSTR_DATALEN);
				)

				busState = IDLE;
				rc = 2;
			}
			else if (*recv == NTBUS_CMD_GETBOARDSTR)
			{
				WITH_USART0_TX_ENABLED(
					writeFrame((uint8_t*) boardStr, NTBUS_CMDGETBOARDSTR_DATALEN);
				)

				busState = IDLE;
				rc = 2;
			}
			else
			{
				rc = 1;    // execute actions of the specific NtNode type.
			}
			break;
		}
	}
	return rc;
}


NtNodeImu::NtNodeImu(uint8_t id, const char* board, NtRingBuf* buffer, tNTBusGetImuData* imudata, uint16_t model) :
		NtNode(id, board, buffer),
		mImudata(imudata),
		modelCode(model)
{

}

NtNodeImu NtNodeImu::getNodeWithNtBuffer(uint8_t id, const char* board, tNTBusGetImuData* imudata, uint16_t model)
{
	NtNodeImu ntNodeImu = NtNodeImu(id, board, &ntBuffer, imudata, model);
	return ntNodeImu;
}

int8_t NtNodeImu::processBusData(uint8_t* recv)
{
	int8_t rc = NtNode::processBusData(recv);
	if (rc == 1)
	{
		if (busState == GETDATA)
		{
			WITH_USART0_TX_ENABLED(
				writeFrame((uint8_t*) mImudata, NTBUS_GETIMU_DATALEN);
			)

			busState = IDLE;    // needs to be re-triggered for more data
			rc = 2;
		}
		else if (busState == COMMANDED)
		{
			if (*recv == NTBUS_CMD_GETSTATUS)
			{
				tNTBusCmdGetStatusData statusData;
				statusData.Status = NTBUS_IMU_STATUS_IMU_PRESENT;
				statusData.State = mImudata->ImuStatus;

				WITH_USART0_TX_ENABLED(
					writeFrame((uint8_t*) &statusData, NTBUS_CMDGETSTATUS_DATALEN);
				)
			}
			else if (*recv == NTBUS_CMD_GETCONFIGURATION)
			{
				WITH_USART0_TX_ENABLED(
					writeFrame((uint8_t*) &modelCode, NTBUS_CMDGETCONFIGURATION_DATALEN);
				)
			}

			busState = IDLE;    // keep FSM in sync even if byte is missed.
			rc = 2;
		}
	}
	return rc;
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
		usart0_write(*frame);
		crc ^= *frame;
        frame++;
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
	// interrupt routines are atomic so the push is thread safe.
	ntBuffer.push(UDR0);
}
