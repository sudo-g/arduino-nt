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
    memset(ringbuf.slots, 0, sizeof(ringbuf.slots));
	assert(ringbuf.wrtInd == 0);
	assert(ringbuf.unread == 0);

	const unsigned int ringBufIndexMask = sizeof(ringbuf.slots) - 1;

	// write some data to the ring buffer
	ringbuf.slots[ringbuf.wrtInd++ & ringBufIndexMask] = 2;
	ringbuf.unread++;

	// test reading from ring buffer
	uint8_t buffer;
	assert(ringbuf.pop(&buffer));
	assert(buffer == 2);
	assert(ringbuf.unread == 0);
    
    std::cout << "[PASS]" << std::endl;
}

void ntRingBufPopWhenEmptyTest()
{
    std::cout << "ntRingBufPopWhenEmptyTest\t";
    
	// initialize ring buffer
	NtRingBuf ringbuf = NtRingBuf();
	ringbuf.wrtInd = 3;

	uint8_t buffer;
	assert(!ringbuf.pop(&buffer));
	assert(ringbuf.unread == 0);
	assert(ringbuf.wrtInd == 3);
    
    std::cout << "[PASS]" << std::endl;
}

void ntRingBufPopLoopTest()
{
    std::cout << "ntRingBufPopLoopTest\t";
    
	NtRingBuf ringbuf = NtRingBuf();
    memset(ringbuf.slots, 0, sizeof(ringbuf.slots));
    
    const unsigned int ringBufIndexMask = sizeof(ringbuf.slots) - 1;
    
	ringbuf.wrtInd = NTBUS_BUFSIZE - 1;
	ringbuf.slots[ringbuf.wrtInd++ & ringBufIndexMask] = 1;
    ringbuf.unread++;

	uint8_t buffer;
	assert(ringbuf.pop(&buffer));
	assert(buffer == 1);
    
    std::cout << "[PASS]" << std::endl;
}
