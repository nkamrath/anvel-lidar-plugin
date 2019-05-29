#ifndef SENSOR_PACKETS_H_
#define SENSOR_PACKETS_H_

#include <stdint.h>

enum class SensorPacketType
{
	GPS,
	IMU,
	LIDAR
};

struct SensorDataPacket
{
	uint16_t SequenceNumber;
	double Timestamp;
	double LocationX;
	double LocationY;
	double LocationZ;
	SensorPacketType PacketType;
	uint16_t PayloadLength;
	uint8_t Payload[];
} __attribute__((packed));

#endif