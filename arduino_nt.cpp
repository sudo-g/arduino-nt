#include <string.h>
#include <Wire.h>
#include "MPU6050.h"
#include "nt.h"

#define NT_TASK_RATE 95

int main(void)
{
	tNTBusGetImuData imuData;
	memset(&imuData, 0, sizeof(imuData));
	imuData.ImuStatus = NTBUS_IMU_IMUSTATUS_BASE | NTBUS_IMU_IMUSTATUS_GYRODATA_OK | NTBUS_IMU_IMUSTATUS_ACCDATA_OK;

	sei();

	Wire.begin();

	MPU6050 imu = MPU6050();
	imu.initialize();
	imu.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);
	imu.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);
	imu.setExternalFrameSync(MPU6050_EXT_SYNC_DISABLED);
	imu.setDLPFMode(MPU6050_DLPF_BW_256);
	imu.setRate(0);    // sample at full speed

	NtNodeImu ntNode = NtNodeImu::getNodeWithNtBuffer(NTBUS_ID_IMU1, "ArduNT IMU", &imuData, NTBUS_IMU_CONFIG_MPU6000);

	uint8_t dataProcessCount = 0;
	for (;;)
	{
		if (dataProcessCount >= NT_TASK_RATE)
		{
			// lower priority and thus occurs less frequent than NT tasks
			imu.getMotion6WithTemp(&imuData.AccX, &imuData.AccY, &imuData.AccZ, &imuData.Temp, &imuData.GyroX, &imuData.GyroY, &imuData.GyroZ);

			dataProcessCount = 0;
		}

		uint8_t recv;
		ntNode.processBusData(&recv);

		dataProcessCount++;
	}

	return 0;
}
