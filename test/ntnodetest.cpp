#include <assert.h>
#include <iostream>
#include <avr/io.h>
#include <string.h>
#include "nt.h"
#include "avrtestutil.h"

void ntNodeMatchIdTest()
{
    std::cout << "ntNodeMatchIdTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    assert(ntNode.matchIdGetData == 0xB1);
    assert(ntNode.getBusState() == NtNode::IDLE);
    assert((UCSR0B & (1<<RXEN0)) && (UCSR0B & (1<<RXCIE0)));
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeNameTest()
{
    std::cout << "ntNodeMatchIdTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    char boardStr[NTBUS_CMDGETBOARDSTR_DATALEN];
    memset(boardStr, 0, NTBUS_CMDGETBOARDSTR_DATALEN);
    memcpy(boardStr, "test", sizeof("test"));
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, boardStr, &buffer);
    
    char actualBoardStr[NTBUS_CMDGETBOARDSTR_DATALEN];
    memset(actualBoardStr, 0, NTBUS_CMDGETBOARDSTR_DATALEN);
    ntNode.writeBoardStr(actualBoardStr);
    
    assert(memcmp(boardStr, actualBoardStr, NTBUS_CMDGETBOARDSTR_DATALEN) == 0);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeTriggerPositiveTest()
{
    std::cout << "ntNodeTriggerPositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
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
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
    buffer.push(1<<7);
    
    uint8_t recv;
    ntNode.processBusData(&recv);
    assert(ntNode.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToGetdataStateNegativeTest()
{
    std::cout << "ntNodeToGetdataStateNegativeTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv) >= 0);
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeToMotordataStatePositiveTest()
{
    std::cout << "ntNodeToMotordataStatePositiveTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_SET | NTBUS_ID_MOTORALL);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv) >= 0);
    assert(ntNode.getBusState() == NtNode::MOTORDATA);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeUntriggerTest()
{
    std::cout << "ntNodeUntriggerTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_SET | NTBUS_ID_MOTORALL);
    
    for (uint8_t i=0; i<10; i++)
    {
        buffer.push(i);
    }
    
    uint8_t recv;
    while (ntNode.processBusData(&recv) >= 0);
    assert(ntNode.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeGetVersionStrTest()
{
    std::cout << "ntNodeGetVersionStrTest\t";
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, "test", &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_ID_IMU1 | NTBUS_GET);
    buffer.push(NTBUS_CMD_GETVERSIONSTR);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv) >= 0);
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    const uint8_t reference[NTBUS_CMDGETVERSIONSTR_DATALEN] = STORM32NTBUS_VERSIONSTR;
    assert(memcmp(reference, usart0buf, NTBUS_CMDGETVERSIONSTR_DATALEN) == 0);
    
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeGetBoardStrTest()
{
    std::cout << "ntNodeGetBoardStrTest\t";
    
    const char boardName[] = "test board";
    char boardNameBuf[NTBUS_CMDGETBOARDSTR_DATALEN];
    memset(boardNameBuf, 0, sizeof(boardNameBuf));
    memcpy(boardNameBuf, boardName, sizeof(boardName));
    
    NtRingBuf buffer = NtRingBuf();
    NtNode ntNode = NtNode(NTBUS_ID_IMU1, boardNameBuf, &buffer);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_ID_IMU1 | NTBUS_GET);
    buffer.push(NTBUS_CMD_GETBOARDSTR);
    
    uint8_t recv;
    while (ntNode.processBusData(&recv) >= 0);
    assert(ntNode.getBusState() == NtNode::TRIGGERED);
    
    tNTBusCmdGetBoardStrData referenceFrame;
    memset(&referenceFrame, 0, NTBUS_CMDGETBOARDSTR_DATALEN);
    memcpy(&referenceFrame, boardName, sizeof(boardName));
    
    uint8_t reference[NTBUS_CMDGETBOARDSTR_DATALEN+1];
    memcpy(reference, &referenceFrame, NTBUS_CMDGETBOARDSTR_DATALEN);
    reference[NTBUS_CMDGETBOARDSTR_DATALEN] =
        ntcrc((uint8_t*) &referenceFrame, NTBUS_CMDGETBOARDSTR_DATALEN);
    
    assert(memcmp(reference, usart0buf, sizeof(reference)) == 0);
    
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}

void ntCrcCalcTest()
{
    std::cout << "ntCrcCalcTest\t";
    
    uint8_t frame[] = {0x00, 0x01, 0x02, 0x03, 0x04};
    
    assert(0x04 == ntcrc(frame, sizeof(frame)));
    
    std::cout << "[PASS]" << std::endl;
}
