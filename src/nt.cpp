#include "nt.h"
#include <avr/io.h>
#include <stdbool.h>

NtNode::NtNode(uint8_t id)
{
	// initializing class states
	reading = false;

	// setting UART to 2MBaud
	UCSR0A |= (1 << U2X0);
	UBRR0H = 0;
	UBRR0L = 1;

	// set 8 bits, 1 stop, no parity
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

	// enable RX line
	UCSR0B |= (1<<RXEN0);
}

void NtNode::writeImuData(tNTBusGetImuData* data, uint8_t status)
{

}

void NtNode::readFromMaster()
{

}
