#include "avrutil.h"
#include <stdio.h>
#include "avrtestutil.h"

#define TEST_USART_BUFSIZE 256

void usart0_write(uint8_t c)
{
    usart0buf[usart0buf_inx & (TEST_USART_BUFSIZE-1)] = c;
    usart0buf_inx++;
}