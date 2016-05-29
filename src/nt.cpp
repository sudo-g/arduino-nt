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


NtNode::NtNode(uint8_t id)
{
	busState = IDLE;
	matchIdGetData = NTBUS_FROM_MASTER | NTBUS_GET | id;
	buffer = &ntBuffer;

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

void NtNode::processBusData()
{
	switch (busState)
	{
	case IDLE:
		break;
	case TRIGGERED:
		break;
	case GETDATA:
		break;
	case MOTORDATA:
		break;
	}
}

void NtNodeImu::writeImuData(tNTBusGetImuData* data, uint8_t status)
{

}


ISR(USART_RX_vect)
{
	// routine is atomic
	// observed externally, wrtInd and unread are incremented simultaneously
	ntBuffer.slots[ntBuffer.wrtInd++ & BUF_SIZE_MASK] = UDR0;
	ntBuffer.unread++;
}
