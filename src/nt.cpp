#include "nt.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define BUF_SIZE_MASK (NTBUS_BUFSIZE-1)

NtRingBuf::NtRingBuf()
{
	wrtInd = 0;
	unread = 0;
}

bool NtRingBuf::pop(uint8_t* data)
{
	if (unread > 0)
	{
		*data = slots[(wrtInd - unread) & BUF_SIZE_MASK];
		unread--;
		return true;
	}
	else
	{
		return false;
	}
}

// module wide buffer storing incoming serial
static NtRingBuf ntBuffer = NtRingBuf();


NtNode::NtNode(uint8_t id) :
		matchIdGetData(NTBUS_FROM_MASTER | NTBUS_GET | id),
		busState(IDLE),
		buffer(&ntBuffer)
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

uint8_t NtNode::processBusData()
{
	uint8_t recv;
	if (buffer->pop(&recv))
	{
		switch (busState)
		{
		case IDLE:
			if (recv == (NTBUS_STX | NTBUS_TRIGGER))
			{
				busState = TRIGGERED;
			}
			break;
		case TRIGGERED:
			if (recv & (1<<7))
			{
				busState = GETDATA;
			}
			break;
		case GETDATA:
			break;
		case MOTORDATA:
			break;
		}
	}
	return recv;
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


NtNodeImu::NtNodeImu(uint8_t id, tNTBusGetImuData* imudata, uint8_t* imustatus) :
		NtNode(id),
		mImudata(imudata),
		mImustatus(imustatus)
{

}

void NtNodeImu::processBusData()
{
	uint8_t recv = NtNode::processBusData();
	if (busState == GETDATA)
	{
		if (recv == matchIdGetData)
		{
			writeImuData();
		}
		busState = TRIGGERED;
	}
}

void NtNodeImu::writeImuData() const
{
	uint8_t* c;
	uint8_t crc = 0;
	for(uint8_t i=0; i<NTBUS_GETIMU_DATALEN-1; i++)
	{
		c = (uint8_t*) mImudata;
		write(*c++);

		crc ^= *c;
	}
	c = (uint8_t*) mImustatus;
	write(*c++);
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
