#include "UdpRx.h"

#include <iostream>
#include <unistd.h>

using namespace std;

UdpRx::~UdpRx()
{
    close(fd);
}

UdpRx::UdpRx(char* address, uint16_t port)
{
    //this is all linux magic for creating and setting up a UDP socket for getting packets
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		cout << "ERROR fd create" << endl;
		return;
	}

	u_int yes = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0)
	{
		cout << "ERROR set sock opt" << endl;
		return;
	}

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // differs from sender
    addr.sin_port = htons(port);

    if(bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
    	cout << "ERROR bind" << endl;
    	return;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0){
    	cout << "Error set sock opt 2" << endl;
    	return;
    }

}

int UdpRx::Rx(void* buffer, uint16_t size)
{
	socklen_t addrlen = sizeof(addr);
    //linux socket lib rx function, blocking till we get a packet (unless non-blocking flags are set)
    int nbytes = recvfrom(fd, buffer, size, 0, (struct sockaddr *) &addr, &addrlen);
    return nbytes;
}