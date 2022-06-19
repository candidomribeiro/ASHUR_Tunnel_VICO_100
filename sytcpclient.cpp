#include "sytcpclient.h"

SYTCPClient::SYTCPClient(int IP_version, const char *IP_Address, int ServerPort)
{
       struct timeval tv;
       tv.tv_sec = 20;  /* 20 Secs Timeout */
       tv.tv_usec = 0;

       int hSocket;
       //Create socket
       hSocket = SocketCreate(IP_version);
       if(hSocket == -1)
       {
           perror("Could not create socket\n");
           exit(1);
       }
       //Connect to remote server
       if (SocketConnect(hSocket, ServerPort, IP_Address, IP_version) < 0)
       {
           perror("connect failed.\n");
           exit(2);
       }
       if(setsockopt(this->socketDescriptor, SOL_SOCKET, SO_SNDTIMEO, static_cast<void *>(&tv), sizeof(tv)) < 0)
       {
           perror("setsockopt fail !!\n");
           exit(3);
       }
}

SYTCPClient::~SYTCPClient()
{
    shutdown(socketDescriptor, SHUT_RDWR);
    close(socketDescriptor);
}

int SYTCPClient::SocketCreate(int IP_version)
{
    int hSocket;
    int domain   = AF_INET;
    int type     = SOCK_STREAM;
    int protocol = IPPROTO_TCP;

    if (IP_version == 6)
    {
        domain = AF_INET6; // IPv6 Socket
    }

    hSocket = socket(domain, type, protocol);
    return hSocket;
}

int SYTCPClient::SocketConnect(int hSocket, int ServerPort, const char *IP_Addr, int IP_version)
{
    int iRetval=-1;
    int version = AF_INET;

    if (IP_version == 6)
    {
        version = AF_INET6;
    }

    struct sockaddr_in remote = {};

    inet_pton(version, IP_Addr, &remote.sin_addr.s_addr);
    remote.sin_family = static_cast<unsigned short>(version);
    remote.sin_port = htons(static_cast<unsigned short>(ServerPort));

    iRetval = connect(hSocket, reinterpret_cast<struct sockaddr *>(&remote), sizeof(struct sockaddr_in));

    this->socketDescriptor = hSocket;
    return iRetval;
}
