#include "nt.h"

NtBus::NtBus(tNTBusGetImuData* imuBuffer)
{
	// TODO: initialize serial port
	imuDataRef = imuBuffer;
}

void NtBus::writeImuData(uint8_t status)
{

}

void NtBus::readFromMaster()
{

}
