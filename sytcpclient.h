#ifndef SYTCPCLIENT_H
#define SYTCPCLIENT_H

#include <QtCore/qglobal.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<time.h>

#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<arpa/inet.h>

class SYTCPClient
{
public:
    SYTCPClient  (int IP_version, const char *IP_Address, int ServerPort);
    virtual ~SYTCPClient ();

    virtual ssize_t clientSend(const void *msg, size_t msgSize) = 0;
    virtual ssize_t clientReceive(void *msg, size_t msgSize) = 0;

private:
    int SocketCreate  (int IP_version);
    int SocketConnect (int hSocket, int ServerPort, const char *IP_Addr, int IP_version);

protected:
    int socketDescriptor;
};

#endif // SYTCPCLIENT_H
