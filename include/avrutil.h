#ifndef _AVRUTIL_H_
#define _AVRUTIL_H_

#include <stdint.h>

#define WITH_USART0_TX_ENABLED(actions) UCSR0B |= (1<<TXEN0); {actions} UCSR0B &= ~(1<<TXEN0);

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * Transmit character over USART.
     *
     * \param c Character to transmit.
     */
    void usart0_write(uint8_t c);

#ifdef __cplusplus
}
#endif

#endif
