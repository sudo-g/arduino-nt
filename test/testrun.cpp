#include <iostream>
#include "ntringbuftest.h"
#include "ntnodetest.h"
#include "ntnodeimutest.h"

int main(int argc, char *argv[])
{
    std::cout << "Executing arduino-nt tests..." << std::endl;
    std::cout << std::endl << "Executing ntRingBuf suite of tests..." << std::endl;
    ntRingBufPopFromBeginTest();
    ntRingBufPopWhenEmptyTest();
    ntRingBufPopLoopTest();
    
    std::cout << std::endl << "Executing ntNode suite of tests..." << std::endl;
    ntNodeMatchIdTest();
    ntNodeNameTest();
    ntNodeTriggerPositiveTest();
    ntNodeTriggerNegativeTest();
    ntNodeToGetdataStatePositiveTest();
    ntNodeToGetdataStateNegativeTest();
    ntNodeToMotordataStatePositiveTest();
    ntNodeUntriggerTest();
    ntNodeGetVersionStrTest();
    ntCrcCalcTest();
    
    std::cout << std::endl << "Executing ntNodeImu suite of tests..." << std::endl;
    ntNodeImuTriggerPositiveTest();
    ntNodeImuTriggerNegativeTest();
    ntNodeImuGetstatusTest();
    ntNodeImuGetconfigurationTest();
    
    std::cout << std::endl;
    
    return EXIT_SUCCESS;
}