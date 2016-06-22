#include <assert.h>
#include <iostream>
#include <string.h>
#include <avr/io.h>
#include "nt.h"
#include "avrtestutil.h"

void ntNodeImuTriggerPositiveTest()
{
    std::cout << "ntNodeImuTriggerPositiveTest\t";
    
    tNTBusGetImuData imudata;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
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
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    buffer.push(NTBUS_STX);
    
    uint8_t recv;
    ntNodeImu.processBusData(&recv);
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeImuEmptyCommand()
{
    std::cout << "ntNodeEmptyCommand\t";
    
    tNTBusGetImuData imudata;
    imudata.AccX = 1;
    imudata.AccY = 2;
    imudata.AccZ = 3;
    imudata.GyroX = 5;
    imudata.GyroY = 6;
    imudata.GyroZ = 4;
    imudata.Temp = 7;
    imudata.ImuStatus = NTBUS_IMU_IMUSTATUS_BASE | NTBUS_IMU_IMUSTATUS_GYRODATA_OK;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_CMD | NTBUS_ID_IMU1);
    buffer.push(0xFF);
    
    uint8_t recv;
    while (ntNodeImu.processBusData(&recv) >= 0);
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
}

void ntNodeImuGetstatusTest()
{
    std::cout << "ntNodeImuGetstatusTest\t";
    
    const uint8_t imustat = NTBUS_IMU_IMUSTATUS_BASE | NTBUS_IMU_IMUSTATUS_GYRODATA_OK;
    
    tNTBusGetImuData imudata;
    imudata.AccX = 1;
    imudata.AccY = 2;
    imudata.AccZ = 3;
    imudata.GyroX = 5;
    imudata.GyroY = 6;
    imudata.GyroZ = 4;
    imudata.Temp = 7;
    imudata.ImuStatus = imustat;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    // go into COMMANDED mode.
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_CMD | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv) >= 0);
    assert(ntNodeImu.getBusState() == NtNode::COMMANDED);
    
    // request status data.
    buffer.push(NTBUS_CMD_GETSTATUS);
    ntNodeImu.processBusData(&recv);
    assert(!(UCSR0B & (1<<TXEN0)));
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    // build reference data
    tNTBusCmdGetStatusData referenceFrame;
    referenceFrame.Status = NTBUS_IMU_STATUS_IMU_PRESENT;
    referenceFrame.State = imustat;
    
    uint8_t referenceOut[NTBUS_CMDGETSTATUS_DATALEN + 1];
    memcpy(&referenceOut, &referenceFrame, NTBUS_CMDGETSTATUS_DATALEN);
    referenceOut[NTBUS_CMDGETSTATUS_DATALEN] =
    ntcrc(
          (uint8_t*) &referenceFrame,
          NTBUS_CMDGETSTATUS_DATALEN
          );
    
    // check USART buffer values match reference
    assert(memcmp(&referenceOut, usart0buf, sizeof(referenceOut)) == 0);
    
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeImuGetconfigurationTest()
{
    std::cout << "ntNodeImuToGetconfigurationTest\t";
    
    tNTBusGetImuData imudata;
    imudata.AccX = 1;
    imudata.AccY = 2;
    imudata.AccZ = 3;
    imudata.GyroX = 5;
    imudata.GyroY = 6;
    imudata.GyroZ = 4;
    imudata.Temp = 7;
    
    const uint16_t imuconfig = NTBUS_IMU_CONFIG_MPU6000;
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, imuconfig);
    
    // go into COMMANDED mode.
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_CMD | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv) >= 0);
    assert(ntNodeImu.getBusState() == NtNode::COMMANDED);
    
    // request configuration data.
    buffer.push(NTBUS_CMD_GETCONFIGURATION);
    ntNodeImu.processBusData(&recv);
    assert(!(UCSR0B & (1<<TXEN0)));
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    // build reference data
    tNTBusCmdGetConfigurationData referenceFrame;
    referenceFrame.Configuration = imuconfig;
    
    uint8_t referenceOut[NTBUS_CMDGETCONFIGURATION_DATALEN + 1];
    memcpy(&referenceOut, &referenceFrame, NTBUS_CMDGETCONFIGURATION_DATALEN);
    referenceOut[NTBUS_CMDGETCONFIGURATION_DATALEN] =
    ntcrc(
        (uint8_t*) &referenceFrame,
        NTBUS_CMDGETCONFIGURATION_DATALEN
    );
    
    // check USART buffer values match reference
    assert(memcmp(&referenceOut, usart0buf, sizeof(referenceOut)) == 0);
           
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeImuGetImuTest()
{
    std::cout << "ntNodeImuGetImuTest\t";
    
    tNTBusGetImuData imudata;
    imudata.AccX = 1;
    imudata.AccY = 2;
    imudata.AccZ = 3;
    imudata.GyroX = 5;
    imudata.GyroY = 6;
    imudata.GyroZ = 4;
    imudata.Temp = 7;
    imudata.ImuStatus =
        NTBUS_IMU_IMUSTATUS_BASE | NTBUS_IMU_IMUSTATUS_GYRODATA_OK | NTBUS_IMU_IMUSTATUS_ACCDATA_OK;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    // go into GETDATA mode.
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv) >= 0);
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    uint8_t referenceOut[NTBUS_GETIMU_DATALEN+1];
    memcpy(&referenceOut, &imudata, NTBUS_GETIMU_DATALEN);
    referenceOut[NTBUS_GETIMU_DATALEN] = ntcrc((uint8_t*) &imudata, NTBUS_GETIMU_DATALEN);
    
    // check USART buffer values match reference
    assert(memcmp(&referenceOut, usart0buf, sizeof(referenceOut)) == 0);
    
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}
