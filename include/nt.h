#ifndef _NT_H_
#define _NT_H_

#include <stdint.h>

//is used by NT Logger to mark log files with a header, from which also the NT communication version can be retrieved
#define STORM32NTBUS_VERSION             003
#define STORM32NTBUS_VERSIONSTR          "v0.03"

#define NTBUS_BUFSIZE                     16

// NT MODULE ID LIST
#define NTBUS_ID_ALLMODULES               0
#define NTBUS_ID_IMU1                     1
#define NTBUS_ID_IMU2                     2
#define NTBUS_ID_MOTORALL                 3
#define NTBUS_ID_MOTORPITCH               4
#define NTBUS_ID_MOTORROLL                5
#define NTBUS_ID_MOTORYAW                 6
#define NTBUS_ID_CAMERA                   7
#define NTBUS_ID_LOGGER                   11

// STARTBYTE flags and masks
#define NTBUS_STX                         0x80 // 0b 1000 0000
#define NTBUS_SFMASK                      0x70 // 0b 0111 0000
#define NTBUS_IDMASK                      0x0F // 0b 0000 1111

#define NTBUS_FLASH                       0x70 // 0b 0111 0000
#define NTBUS_RESET                       0x50 // 0b 0101 0000   -> 0xD0
#define NTBUS_SET                         0x40 // 0b 0100 0000
#define NTBUS_GET                         0x30 // 0b 0011 0000   -> 0xB0
#define NTBUS_TRIGGER                     0x10 // 0b 0001 0000   -> 0x90
#define NTBUS_CMD                         0x00 // 0b 0000 0000

// COMMANDS , this is the command byte after a NTBUS_STX|NTBUS_CMD|NTBUS_ID startbyte
typedef enum {
	// general cmds
	NTBUS_CMD_GETSTATUS = 1,
	NTBUS_CMD_GETVERSIONSTR,           //must be supported by any NT module
	NTBUS_CMD_GETBOARDSTR,             //must be supported by any NT module
	NTBUS_CMD_GETCONFIGURATION,        //must be supported by any NT module

	NTBUS_CMD_PIDDATA,                 //35
	NTBUS_CMD_PARAMETERDATA,           //36
	NTBUS_CMD_AHRS1DATA,               //37
	NTBUS_CMD_AHRS2DATA,
	NTBUS_CMD_ACCGYRO1RAWDATA_V2,      //40
	NTBUS_CMD_ACCGYRO2RAWDATA_V2,
	NTBUS_CMD_ACCGYRO3RAWDATA_V2,
	NTBUS_CMD_ACCGYRO1DATA_V2,         //43
	NTBUS_CMD_ACCGYRO2DATA_V2,

	NTBUS_CMD_READSETUP = 120,
	NTBUS_CMD_WRITESETUP,
	NTBUS_CMD_STORESETUP,

	NTBUS_CMD_NOTUSED = 0xFF
} NTBUSCMDTYPE;

// NTBUS_CMD_GETSTATUS
// from STorM32: stx, cmdbyte -> to STorM32: 2 x u8 cmddata, crc
typedef struct {
	char Status;
	uint8_t State;
} tNTBusCmdGetStatusData;

#define NTBUS_CMDGETSTATUS_DATALEN        (sizeof(tNTBusCmdGetStatusData))

// NTBUS_CMD_GETVERSIONSTR
// from STorM32: stx, cmdbyte -> to STorM32: 16 x u8 cmddata, crc
typedef struct {
	char VersionStr[16];
} tNTBusCmdGetVersionStrData;

#define NTBUS_CMDGETVERSIONSTR_DATALEN    (sizeof(tNTBusCmdGetVersionStrData))

// NTBUS_CMD_GETBOARDSTR
// from STorM32: stx, cmdbyte -> to STorM32: 16 x u8 cmddata, crc
typedef struct{
	char BoardStr[16];
} tNTBusCmdGetBoardStrData;

#define NTBUS_CMDGETBOARDSTR_DATALEN      (sizeof(tNTBusCmdGetBoardStrData))

// NTBUS_CMD_GETCONFIGURATION
// from STorM32: stx, cmdbyte -> in STorM32: 1 x u16 cmddata, crc
typedef struct {
	uint16_t Configuration;
} tNTBusCmdGetConfigurationData;

#define NTBUS_CMDGETCONFIGURATION_DATALEN (sizeof(tNTBusCmdGetConfigurationData))


// IMU MODULE
// SET: -
// GET: 3xi16 acc + 3xi16 gyro + 1xi16 temp + 1xu8 imu status

// status byte , is used for both internal bookkeeping and CMD command
// response to NTBUS_CMD_GETSTATUS
#define NTBUS_IMU_STATUS_IMU_PRESENT      0x80

// configuration word
// response to NTBUS_CMD_GETCONFIGURATION
#define NTBUS_IMU_CONFIG_MPU6000          0x0002
#define NTBUS_IMU_CONFIG_MPU9250          0x0004
#define NTBUS_IMU_CONFIG_MODELUNKNOWN     0x0000
#define NTBUS_IMU_CONFIG_MODELMASK        0x0007

#define NTBUS_IMU_CONFIG_OVERSAMPLED      0x0010
#define NTBUS_IMU_CONFIG_FASTGYRO         0x0020
#define NTBUS_IMU_CONFIG_FASTACC          0x0040
#define NTBUS_IMU_CONFIG_FAST             (NTBUS_IMU_CONFIG_FASTGYRO|NTBUS_IMU_CONFIG_FASTACC)

// GET IMU imu status byte
// response to NTBUS_GET IMU
// also used in NTBUS_CMD_ACCGYRODATA
#define NTBUS_IMU_IMUSTATUS_BASE          0x01  //must always be set, so that status is always >0
#define NTBUS_IMU_IMUSTATE_OK             0x02  //this one is injected, comes from the STorM32 controller, not used by NT imu
#define NTBUS_IMU_IMUSTATUS_GYRODATA_OK   0x04
#define NTBUS_IMU_IMUSTATUS_ACCDATA_OK    0x08
#define NTBUS_IMU_IMUSTATUS_GYRO_CLIPPED  0x10
#define NTBUS_IMU_IMUSTATUS_ACC_CLIPPED   0x20

// NTBUS_GET
// to STorM32: 6xi16 + 1xi16 + 1xu8 + crc = 15+1 bytes
typedef struct {
	int16_t AccX;  //+-4g
	int16_t AccY;  //+-4g
	int16_t AccZ;  //+-4g
	int16_t GyroX; //+-1000°/s
	int16_t GyroY; //+-1000°/s
	int16_t GyroZ; //+-1000°/s
	int16_t Temp;
	uint8_t ImuStatus;
} tNTBusGetImuData;

#define NTBUS_GETIMU_DATALEN              (sizeof(tNTBusGetImuData))


/**
 * Ring buffer used for NT bus data.
 */
class NtRingBuf
{
public:
	NtRingBuf();

	/**
	 * Get the last unread byte from buffer.
	 *
	 * \param out Buffer to write output to.
	 * \return True if data was read, False if no unread items.
	 */
	bool pop(uint8_t* out);

	/**
	 * Write a byte into the buffer.
	 *
	 * \param data Data to write to the buffer.
	 */
	void push(uint8_t data);

	/**
	 * \return the number of unread bytes.
	 */
	uint8_t getUnreadBytes() const;

private:
	uint8_t slots[NTBUS_BUFSIZE];
	volatile uint8_t wrtInd;
	volatile uint8_t unread;
};


/**
 * Abstract NT bus node.
 */
class NtNode
{
public:
	enum NtState
	{
		IDLE, TRIGGERED, GETDATA, COMMANDED
	};

	const uint8_t matchIdGetData;
	const uint8_t matchIdCommand;

	/**
	 * Creates a NtNode which matches a specified ID.
	 *
	 * \param id     NT ID which this node responds to.
	 * \param board  String name of the board.
	 * \param buffer The ring buffer to read serial from.
	 */
	NtNode(uint8_t id, const char* board, NtRingBuf* buffer);

	/**
	 * Writes the board string to buffer.
	 *
	 * \param buf Buffer to write the board string to.
	 */
	void writeBoardStr(char* buf) const;

	/**
	 * \return The state of the NT bus.
	 */
	NtState getBusState() const;

	/**
	 * Handle data written to the bus.
	 *
	 * \param recv Byte received from the serial buffer.
	 * \return 0 if data read, 1 if pass control to sub-class, 2 if data transmitted, -1 if no data read.
	 */
	int8_t processBusData(uint8_t* data);

protected:
	NtState busState = IDLE;

private:
	char boardStr[NTBUS_CMDGETBOARDSTR_DATALEN];
	NtRingBuf* buffer;
};


class NtNodeImu : public NtNode
{
public:
	/**
	 * Creates a IMU NtNode which matches a specified ID.
	 *
	 * \param id        NT ID of this IMU node.
	 * \param buffer    The ring buffer to read serial from.
	 * \param imudata   Descriptor of IMU measurements.
	 * \param model     Model of IMU used.
	 */
	NtNodeImu(uint8_t id, const char* board, NtRingBuf* buffer, tNTBusGetImuData* imudata, uint16_t model);

	static NtNodeImu getNodeWithNtBuffer(uint8_t id, const char* board, tNTBusGetImuData* imudata, uint16_t model);

	/**
	 * Handle data written to the bus.
	 *
	 * \param recv Byte received from the serial buffer.
	 * \return 0 if data read, 1 if pass control to sub-class, 2 if data transmitted, -1 if no data read.
	 */
	int8_t processBusData(uint8_t* data);

private:
	const tNTBusGetImuData* mImudata;
	const uint16_t modelCode;

	void writeImuData() const;
};

/**
 * Calculate checksum of a NT frame.
 *
 * \param frame  Frame data.
 * \param length The length of the frame.
 * \return Checksum value of the frame.
 */
uint8_t ntcrc(uint8_t* frame, uint8_t length);

#endif
