#ifndef _AVRTESTUTIL_H_
#define _AVRTESTUTIL_H_

#include <pthread.h>

#define TEST_USART_BUFSIZE 256

extern pthread_mutex_t usart_write_lock;
extern uint8_t usartbuf[TEST_USART_BUFSIZE];

/**
 * Starts simulation of UART0 hardware.
 *
 * \return Non-zero if success, 0 for failure.
 */
int usart0_init(void);

#endif