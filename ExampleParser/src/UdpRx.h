#ifndef UDP_RX_H_
#define UDP_RX_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class UdpRx
{
	public:
		UdpRx(char* address, uint16_t port);
		~UdpRx();
		int Rx(void* buffer, uint16_t length);

	private:
		int fd; //socket descriptor handle
		struct sockaddr_in addr; //socket rx address structure
};

#endif