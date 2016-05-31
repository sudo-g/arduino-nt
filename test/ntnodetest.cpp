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

void ntNodeTriggerPositiveTest()
{
    std::cout << "ntNodeTriggerPositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    
    uint8_t recv;
    ntNode.processBusData(&recv);
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeTriggerNegativeTest()
{
    std::cout << "ntNodeTriggerNegativeTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(1<<7);
    
    uint8_t recv;
    ntNode.processBusData(&recv);
    assert(ntNode.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToGetdataStatePositiveTest()
{
    std::cout << "ntNodeToGetdataStatePositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET);
    
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
    buffer.push(NTBUS_STX);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv));
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToMotordataStatePositiveTest()
{
    std::cout << "ntNodeToMotordataStatePositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_ID_IMU1 | NTBUS_GET);
    buffer.push(0x42);
    buffer.push(NTBUS_STX | NTBUS_SET | NTBUS_ID_MOTORALL);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv));
    assert(ntNode.getBusState() == NtNode::MOTORDATA);
    
    std::cout << "[PASS]" << std::endl;
}
