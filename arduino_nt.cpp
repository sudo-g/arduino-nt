#include <string.h>
#include <Wire.h>
#include "MPU6050.h"
#include "nt.h"

int main(void)
{
	tNTBusGetImuData imuData;
	memset(&imuData, 0, sizeof(imuData));
	imuData.ImuStatus = NTBUS_IMU_IMUSTATUS_BASE | NTBUS_IMU_IMUSTATUS_GYRODATA_OK | NTBUS_IMU_IMUSTATUS_ACCDATA_OK;

	Wire.begin();

	MPU6050 imu = MPU6050();
	imu.initialize();
	imu.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);
	imu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	imu.setExternalFrameSync(MPU6050_EXT_SYNC_DISABLED);
	imu.setDLPFMode(MPU6050_DLPF_BW_256);
	imu.setRate(0);    // sample at full speed

	NtNodeImu ntNode = NtNodeImu::getNodeWithNtBuffer(NTBUS_ID_IMU1, "ArduNT IMU", &imuData, NTBUS_IMU_CONFIG_MPU6000);

	for (;;)
	{
		imu.getMotion6(&imuData.AccX, &imuData.AccY, &imuData.AccZ, &imuData.GyroX, &imuData.GyroY, &imuData.GyroZ);
		imuData.Temp = imu.getTemperature();
		uint8_t recv;
		ntNode.processBusData(&recv);
	}

	sei();

	return 0;
}
