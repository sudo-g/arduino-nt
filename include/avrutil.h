#ifndef _AVRUTIL_H_
#define _AVRUTIL_H_

#include <stdint.h>

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
