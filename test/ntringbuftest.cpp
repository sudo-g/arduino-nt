#include "ntringbuftest.h"
#include <assert.h>
#include <string.h>
#include <iostream>
#include "nt.h"

void ntRingBufPopFromBeginTest()
{
    std::cout << "ntRingBufPopFromBeginTest\t";
    
	// initialize ring buffer
	NtRingBuf ringbuf = NtRingBuf();
	assert(ringbuf.getUnreadBytes() == 0);
    
	ringbuf.push(2);

	// test reading from ring buffer
	uint8_t buffer;
	assert(ringbuf.pop(&buffer));
	assert(buffer == 2);
	assert(ringbuf.getUnreadBytes() == 0);
    
    std::cout << "[PASS]" << std::endl;
}

void ntRingBufPopWhenEmptyTest()
{
    std::cout << "ntRingBufPopWhenEmptyTest\t";
    
	// initialize ring buffer
	NtRingBuf ringbuf = NtRingBuf();
    
    ringbuf.push(1);
    ringbuf.push(2);
    
    uint8_t recv;
    ringbuf.pop(&recv);
    ringbuf.pop(&recv);

	uint8_t buffer;
	assert(!ringbuf.pop(&buffer));
	assert(ringbuf.getUnreadBytes() == 0);
    
    std::cout << "[PASS]" << std::endl;
}

void ntRingBufPopLoopTest()
{
    std::cout << "ntRingBufPopLoopTest\t";
    
	NtRingBuf ringbuf = NtRingBuf();
    
    for (unsigned int i=0; i<NTBUS_BUFSIZE; i++)
    {
        ringbuf.push(1);
    }
    ringbuf.push(3);

	uint8_t buffer;
	assert(ringbuf.pop(&buffer));
	assert(buffer == 1);
    
    std::cout << "[PASS]" << std::endl;
}
