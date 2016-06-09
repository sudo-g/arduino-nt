#include "avrutil.h"
#include "avrtestutil.h"
#include <avr/io.h>
#include <pthread.h>

void usart0_write(uint8_t c)
{
    // prevent cases where UDR has not changed but TX status is busy.
    pthread_mutex_lock(&usart_write_lock);
    
    UCSR0A |= (1<<TXC0);
    UDR0 = c;
    
    pthread_mutex_unlock(&usart_write_lock);
}