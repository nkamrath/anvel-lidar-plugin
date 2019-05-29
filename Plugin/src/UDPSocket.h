#ifndef UDP_SOCKET_H_
#define UDP_SOCKET_H_

#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "SensorPackets.h"

#include "Core/Vector3.h"

class UDPSocket
{
	public:
		UDPSocket();
		UDPSocket(char* groupAddress, uint16_t port);
		void Send(SensorPacketType PacketType, Anvel::Vector3 SensorLocation, double CurrentTime, void* data, uint16_t length);

	private:
		struct sockaddr_in m_addr;
		int fd;
		uint16_t m_nextSequenceNumber = 0;
};

#endif