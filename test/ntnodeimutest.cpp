#include <assert.h>
#include <iostream>
#include "nt.h"

void ntNodeImuTriggerPositiveTest()
{
    std::cout << "ntNodeImuTriggerPositiveTest\t";
    
    tNTBusGetImuData imudata;
    uint8_t imustatus;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, &imustatus);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    
    uint8_t recv;
    ntNodeImu.processBusData(&recv);
    assert(ntNodeImu.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeImuTriggerNegativeTest()
{
    std::cout << "ntNodeImuTriggerPositiveTest\t";
    
    tNTBusGetImuData imudata;
    uint8_t imustatus;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, &imustatus);
    
    buffer.push(1<<7);
    
    uint8_t recv;
    ntNodeImu.processBusData(&recv);
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}