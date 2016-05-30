#include <iostream>
#include "ntringbuftest.h"
#include "ntnodetest.h"

int main(int argc, char *argv[])
{
    std::cout << "Executing arduino-nt tests..." << std::endl;
    std::cout << std::endl << "Executing ntRingBuf suite of tests..." << std::endl;
    ntRingBufPopFromBeginTest();
    ntRingBufPopWhenEmptyTest();
    ntRingBufPopLoopTest();
    
    std::cout << std::endl << "Executing ntNode suite of tests..." << std::endl;
    ntNodeMatchIdTest();
    ntNodeTriggerTest();
    ntNodeToGetdataStatePositiveTest();
    ntNodeToGetdataStateNegativeTest();
    
    std::cout << std::endl;
    
    return EXIT_SUCCESS;
}