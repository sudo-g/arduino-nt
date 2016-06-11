#include <pthread.h>
#include <avr/io.h>
#include "avrtestutil.h"

pthread_mutex_t usart_write_lock;

uint8_t usartbuf[TEST_USART_BUFSIZE];
unsigned int usartbuf_inx = 0;

void *usarthwrite_task(void* arg)
{
    for (;;)
    {
        pthread_mutex_lock(&usart_write_lock);
        // if TX status marked as busy
        if (UCSR0A & (1<<TXC0))
        {
            pthread_mutex_unlock(&usart_write_lock);
            usartbuf[usartbuf_inx & (TEST_USART_BUFSIZE-1)] = UDR0;
            
            // TX status longer busy
            UCSR0A &= (1<<TXC0);
        }
        else
        {
            pthread_mutex_unlock(&usart_write_lock);
        }
    }
    
    return NULL;
}

int usart0_init(void)
{
    pthread_t uarthw_sim;
    return pthread_create(&uarthw_sim, NULL, usarthwrite_task, NULL);
}