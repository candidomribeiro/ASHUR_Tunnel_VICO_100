#ifndef ASHURTUNNELVICO100_H
#define ASHURTUNNELVICO100_H

#include "ASHURtunnelVICO100_global.h"

#include "sytcpclient.h"
#include <byteswap.h>
#include <cstring>

struct MsgReceived
{
    unsigned char  address;
    unsigned char  function;
    unsigned char  size;
    char           data[253];
    unsigned short ashur_int_value;
    double         ashur_double_value;
    //unsigned short crc;
};

class ASHURTUNNELVICO100_EXPORT ASHURtunnelVICO100 : public SYTCPClient
{
private:
    char *MessageSnd = nullptr;
    char *MessageRcv = nullptr;
    unsigned short gen_crc       (const char *buffer, long int buffer_length);
    unsigned short modbusRTU_crc (const char *buffer, long int buffer_length);
    ssize_t        clientSend    (const void *msg, size_t msgSize) override;
    ssize_t        clientReceive (void *msg, size_t msgSize) override;

public:
    ASHURtunnelVICO100(int IP_version, const char *IP_Address, int ServerPort);
    ~ASHURtunnelVICO100() override;

    ssize_t sendMsg   (char address, char function, const char msg[]);
    ssize_t receiveMsg(char *msg);
    ssize_t receiveMsg(MsgReceived *msg);
};

#endif // ASHURTUNNELVICO100_H
