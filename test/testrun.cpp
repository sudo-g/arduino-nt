#include <iostream>
#include "ntringbuftest.h"

int main(int argc, char *argv[])
{
    std::cout << "Executing arduino-nt tests" << std::endl;
    std::cout << "Executing ntRingBuf tests" << std::endl;
    ntRingBufPopFromBeginTest();
    ntRingBufPopWhenEmptyTest();
    ntRingBufPopLoopTest();
    
    return EXIT_SUCCESS;
}