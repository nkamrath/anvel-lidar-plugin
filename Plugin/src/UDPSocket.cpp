
#include "UDPSocket.h"
#include <stdlib.h>

UDPSocket::UDPSocket()
{
	
}

UDPSocket::UDPSocket(char* groupAddress, uint16_t port)
{
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd > 0 && groupAddress)
	{
		memset(&m_addr, 0, sizeof(m_addr));
	    m_addr.sin_family = AF_INET;
	    m_addr.sin_addr.s_addr = inet_addr(groupAddress);
	    m_addr.sin_port = htons(port);
	}
}

void UDPSocket::Send(SensorPacketType PacketType, double CurrentTime, void* data, uint16_t length)
{
	SensorDataPacket* packet = (SensorDataPacket*) malloc(sizeof(SensorDataPacket) + length);

	packet->SequenceNumber = m_nextSequenceNumber;
	m_nextSequenceNumber++;

	packet->Timestamp = CurrentTime;
	packet->PacketType = PacketType;
	packet->PayloadLength = length;
	memcpy(packet->Payload, data, length);

	sendto(fd, packet, sizeof(SensorDataPacket) + length, 0, (struct sockaddr*) &m_addr, sizeof(m_addr));
	free(packet);
}