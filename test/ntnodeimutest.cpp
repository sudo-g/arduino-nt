#include <assert.h>
#include <iostream>
#include <avr/io.h>
#include "nt.h"

void ntNodeImuTriggerPositiveTest()
{
    std::cout << "ntNodeImuTriggerPositiveTest\t";
    
    tNTBusGetImuData imudata;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
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
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    buffer.push(NTBUS_STX);
    
    uint8_t recv;
    ntNodeImu.processBusData(&recv);
    assert(ntNodeImu.getBusState() == NtNode::IDLE);
    
    std::cout << "[PASS]" << std::endl;
}

void ntNodeImuGetstatusTest()
{
    std::cout << "ntNodeImuGetstatusTest\t";
    
    tNTBusGetImuData imudata;
    imudata.AccX = 1;
    imudata.AccY = 2;
    imudata.AccZ = 3;
    imudata.GyroX = 5;
    imudata.GyroY = 6;
    imudata.GyroZ = 4;
    imudata.Temp = 7;
    imudata.ImuStatus = 0x01;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv));
    assert(ntNodeImu.getBusState() == NtNode::GETDATA);
    
    buffer.push(NTBUS_CMD_GETSTATUS);
    ntNodeImu.processBusData(&recv);
    
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
    imudata.ImuStatus = 0x01;
    
    NtRingBuf buffer = NtRingBuf();
    NtNodeImu ntNodeImu = NtNodeImu(NTBUS_ID_IMU1, &buffer, &imudata, NTBUS_IMU_CONFIG_MPU6000);
    
    buffer.push(NTBUS_STX | NTBUS_TRIGGER);
    buffer.push(NTBUS_STX | NTBUS_GET | NTBUS_ID_IMU1);
    
    uint8_t recv;
    memset(&recv, 0, sizeof(recv));
    while (ntNodeImu.processBusData(&recv));
    assert(ntNodeImu.getBusState() == NtNode::GETDATA);
    
    buffer.push(NTBUS_CMD_GETSTATUS);
    ntNodeImu.processBusData(&recv);
    
    std::cout << "[PASS]" << std::endl;
}
