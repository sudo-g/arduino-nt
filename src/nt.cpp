#include "nt.h"
#include <avr/io.h>
#include <avr/interrupt.h>

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
		matchIdGetData(NTBUS_FROM_MASTER | NTBUS_GET | id),
		busState(IDLE),
		buffer(buffer)
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
			// specific NtNode types execute this state's events and resets the state.
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

NtNode::NtState NtNode::getBusState() const
{
	return busState;
}

inline void NtNode::write(uint8_t c) const
{
	while (!UCSR0A & (1<<TXC0));
	UDR0 = c;
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
	if (busState == GETDATA)
	{
		if (*recv == NTBUS_CMD_GETSTATUS)
		{
			// enable TX line
			UCSR0B |= (1<<TXEN0);

			// write data
			// TODO

			// disable TX line
			UCSR0B &= (1<<TXEN0);
		}
		else if (*recv == NTBUS_CMD_GETCONFIGURATION)
		{
			uint8_t* c = (uint8_t*) &modelCode;
			uint8_t crc;
			for (uint8_t i=0; i<NTBUS_CMDGETCONFIGURATION_DATALEN; i++)
			{
				write(*(c++));
				crc ^= *c;
			}
		}
	}
	return ret;
}

void NtNodeImu::writeImuData() const
{
	uint8_t* c = (uint8_t*) mImudata;
	uint8_t crc = 0;
	for(uint8_t i=0; i<NTBUS_GETIMU_DATALEN; i++)
	{
		write(*(c++));
		crc ^= *c;
	}
    
	crc ^= *c;
	write(crc);
}


ISR(USART_RX_vect)
{
	// routine is atomic
	// observed externally, wrtInd and unread are incremented simultaneously
	ntBuffer.slots[ntBuffer.wrtInd++ & BUF_SIZE_MASK] = UDR0;
	ntBuffer.unread++;
}
