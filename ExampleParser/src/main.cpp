#include <iostream>
#include <math.h>
#include "UdpRx.h"

using namespace std;

//packet type provisioning just in case there are multiple sensors we care about
enum class SensorPacketType
{
	GPS,
	IMU,
	LIDAR
};

//sensor data packet structure to grab data from ANVEL plugin
struct SensorDataPacket
{
	uint16_t SequenceNumber;
	double Timestamp;
	SensorPacketType PacketType;
	uint16_t PayloadLength;
	uint8_t Payload[];
} __attribute__((packed));

int main()
{
	//create the UDP socket to listen for the ANVEL plugin output
	UdpRx udprx = UdpRx("239.255.76.67", 7667);

	//create a buffer to store incoming packets
	uint16_t max_length = 65535;
	uint8_t* buffer = (uint8_t*)malloc(max_length);

	//get a pointer to our buffer as a sensor data packet type
	SensorDataPacket* p = (SensorDataPacket*)buffer;

	//loop to grab a set number of lidar packets
	int lidar_packets = 0;
	while(lidar_packets < 8)
	{
		int rx_size = udprx.Rx(buffer, max_length);
		if(rx_size > 0)
		{
			//checm the type to make sure this is actually a lidar data packet
			if(p->PacketType == SensorPacketType::LIDAR)
			{
				//since this is a lidar data packet the payload is an array of doubles x,y,z
				double* lidar_points = (double*)p->Payload;
				for(int i = 0; i < p->PayloadLength/8; i+=3)
				{
					double x = lidar_points[i];
					double y = lidar_points[i+1];
					double z = lidar_points[i+2];

					//throw out "bad" points from anvel lidar scan if any exist
					if(!isnan(x) && !isnan(y) && !isnan(z) && x > -100000 && x < 100000 && y > -100000 && y < 100000 && z > -100000 && z < 100000 && x != 0 && y != 0 && z != 0)
					{
						cout << "valid lidar point!" << endl;
					}
				}

				lidar_packets++;
			}
		}
	}

	delete[] buffer;
}