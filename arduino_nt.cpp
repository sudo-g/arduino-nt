#include "arduino_nt.h"

#include <string.h>
#include <Wire.h>
#include "MPU6050.h"
#include "nt.h"

int main(void)
{
	tNTBusGetImuData imuData;
	memset(&imuData, 0, sizeof(imuData));

	Wire.begin();


	MPU6050 imu = MPU6050();
	imu.initialize();

	NtNodeImu ntNode = NtNodeImu::getNodeWithNtBuffer(NTBUS_ID_IMU1, "ArduNT IMU", &imuData, NTBUS_IMU_CONFIG_MPU6000);

	for (;;)
	{
		imu.getMotion6(&imuData.AccX, &imuData.AccY, &imuData.AccZ, &imuData.GyroX, &imuData.GyroY, &imuData.GyroZ);

		uint8_t recv;
		ntNode.processBusData(&recv);
	}

	sei();

	return 0;
}
