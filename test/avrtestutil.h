#ifndef _AVRTESTUTIL_H_
#define _AVRTESTUTIL_H_

#include <pthread.h>

#define TEST_USART_BUFSIZE 256

#ifdef __cplusplus
extern "C" {
#endif

    extern uint8_t usart0buf[TEST_USART_BUFSIZE];
    extern unsigned int usart0buf_inx;
    
    void usart0_reset(void);
    
#ifdef __cplusplus
}
#endif

#endif