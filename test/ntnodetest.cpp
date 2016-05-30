#include <assert.h>
#include <iostream>
#include "nt.h"

void ntNodeMatchIdTest()
{
    std::cout << "ntNodeMatchIdTest\t";
    
    NtNode ntNode = NtNode::createNtNode(NTBUS_ID_IMU1);
    assert(ntNode.matchIdGetData == 0xB1);
    assert(ntNode.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeTriggerTest()
{
    std::cout << "ntNodeTriggerTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    
    uint8_t recv;
    ntNode.processBusData(&recv);
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToGetdataStatePositiveTest()
{
    std::cout << "ntNodeToGetdataStatePositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv));
    assert(ntNode.getBusState() == NtNode::GETDATA);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToGetdataStateNegativeTest()
{
    std::cout << "ntNodeToGetdataStateNegativeTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv));
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}