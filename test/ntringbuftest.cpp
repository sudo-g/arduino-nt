#include "ntringbuftest.h"
#include <assert.h>
#include "nt.h"

void ntRingBufPopFromBeginTest()
{
	// initialize ring buffer
	NtRingBuf ringbuf = NtRingBuf();
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
}

void ntRingBufPopWhenEmptyTest()
{
	// initialize ring buffer
	NtRingBuf ringbuf = NtRingBuf();
	ringbuf.wrtInd = 3;

	uint8_t buffer;
	assert(!ringbuf.pop(&buffer));
	assert(ringbuf.unread == 0);
	assert(ringbuf.wrtInd == 3);
}

void ntRingBufPopLoopTest()
{
	NtRingBuf ringbuf = NtRingBuf();
	ringbuf.wrtInd = NTBUS_BUFSIZE - 1;
	ringbuf.slots[ringbuf.wrtInd++ & NTBUS_BUFSIZE] = 1;

	uint8_t buffer;
	assert(ringbuf.pop(&buffer));
	assert(buffer == 1);
}
