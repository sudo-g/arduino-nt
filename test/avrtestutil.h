#ifndef _AVRTESTUTIL_H_
#define _AVRTESTUTIL_H_

#include <pthread.h>

extern pthread_mutex_t usart_write_lock;

/**
 * Starts simulation of UART0 hardware.
 *
 * \return Non-zero if success, 0 for failure.
 */
int usart0_init(void);

#endif