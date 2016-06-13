#include "avrtestutil.h"

uint8_t usart0buf[TEST_USART_BUFSIZE];
unsigned int usart0buf_inx = 0;

void usart0_reset(void)
{
    usart0buf_inx = 0;
}