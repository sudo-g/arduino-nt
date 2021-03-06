#include "avrutil.h"
#include <avr/io.h>

inline void usart0_write(uint8_t c)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
	while (!(UCSR0A & (1<<TXC0)));
}
