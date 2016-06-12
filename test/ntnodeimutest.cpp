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
    
    // go into GETDATA mode.
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv));
    assert(ntNodeImu.getBusState() == NtNode::GETDATA);
    
    // request status data.
    buffer.push(NTBUS_CMD_GETSTATUS);
    ntNodeImu.processBusData(&recv);
    
    assert(ntNodeImu.getBusState() == NtNode::TRIGGERED);
    
    // check USART buffer has the correct values.
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
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, "test", &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    // go into GETDATA mode.
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv));
    assert(ntNodeImu.getBusState() == NtNode::GETDATA);
    
    // request status data.
    buffer.push(NTBUS_CMD_GETCONFIGURATION);
    ntNodeImu.processBusData(&recv);
    
    assert(ntNodeImu.getBusState() == NtNode::TRIGGERED);
    
    // check USART buffer has the correct values.
    tNTBusCmdGetConfigurationData referenceFrame;
    referenceFrame.Configuration = imuconfig;
    
    uint8_t referenceOut[NTBUS_CMDGETCONFIGURATION_DATALEN + 1];
    memcpy(&referenceOut, &referenceFrame, NTBUS_CMDGETCONFIGURATION_DATALEN);
    referenceOut[NTBUS_CMDGETCONFIGURATION_DATALEN] =
    ntcrc(
        (uint8_t*) &referenceFrame,
        NTBUS_CMDGETCONFIGURATION_DATALEN
    );
    
    assert(memcmp(&referenceOut, usart0buf, sizeof(referenceOut)) == 0);
           
    usart0_reset();
    
    std::cout << "[PASS]" << std::endl;
}
