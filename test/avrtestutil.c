#include <pthread.h>
#include "avrtestutil.h"

uint8_t uartbuffer

void *usarthwrite_task(void* arg)
{
    
}

int usart0_init(void)
{
    pthread_t uarthw_sim;
    return pthread_create(&uarthw_sim, NULL, usarthwrite_task, NULL);
}