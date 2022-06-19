#include "ashurtunnelvico100.h"

ASHURtunnelVICO100::ASHURtunnelVICO100(int IP_version, const char *IP_Address, int ServerPort)
    :SYTCPClient(IP_version, IP_Address, ServerPort)
{
    MessageSnd = new char[260];
    MessageRcv = new char[260];
}

ASHURtunnelVICO100::~ASHURtunnelVICO100()
{
    delete MessageSnd;
    delete MessageRcv;
}

unsigned short ASHURtunnelVICO100::gen_crc(const char *buffer, long int buffer_length)
{
    unsigned char c, treat, bcrc;
    unsigned short wcrc = 0;
    long int i, j;
    for (i = 0; i < buffer_length; i++)
    {
        memcpy(&c, &buffer[i], 1);
        //c = buffer[i];
        for (j = 0; j < 8; j++)
        {
            treat = c & 0x80;
            c <<= 1;
            bcrc = (wcrc >> 8) & 0x80;
            wcrc <<= 1;
            if (treat != bcrc)
                wcrc ^= 0x1021;
        }
    }
    return wcrc;
}

unsigned short ASHURtunnelVICO100::modbusRTU_crc(const char *buffer, long int buffer_length)
{
    unsigned short crc = 0xFFFF;

    for (long int pos = 0; pos < buffer_length; pos++)
    {
      crc ^= static_cast<unsigned short>(buffer[pos]); // XOR byte into least sig. byte of crc

      for (long int i = 8; i != 0; i--)                // Loop over each bit
      {
        if ((crc & 0x0001) != 0)                       // If the LSB is set
        {
          crc >>= 1;                                   // Shift right and XOR 0xA001
          crc ^= 0xA001;
        }
        else                                           // Else LSB is not set
        {
          crc >>= 1;                                   // Just shift right
        }
      }
    }

    return crc; // BE (Big Endian value)
}

ssize_t ASHURtunnelVICO100::sendMsg(char address, char function, const char msg[])
{
    unsigned short crcbe; // CRC Big Endian
    size_t lenght = 8;    // message lenght

    memcpy(MessageSnd, &address, 1);
    memcpy(MessageSnd + 1, &function, 1);

    memcpy(MessageSnd + 2, msg, 4);

    crcbe = modbusRTU_crc(MessageSnd, static_cast<long int>(lenght - 2));
    //crcbe = bswap_16(crcle);  // Converts LE to BE

    memcpy(MessageSnd + 6, &crcbe, 2);

    return clientSend(MessageSnd, lenght);
}

ssize_t ASHURtunnelVICO100::receiveMsg(char *msg)
{
    ssize_t retval = clientReceive(msg, 256);
    if(retval < 0)
    {
        memcpy(msg, "失败\0",8);
        return retval;
    }

    return retval;
}

ssize_t ASHURtunnelVICO100::receiveMsg(MsgReceived *msg)
{
    char rawmsg[256];
    unsigned char temp_f[4]; // temporary "float"
    unsigned short temp_i, temp_fl, temp_fh; // temporary int
    unsigned int tmp;
    size_t msgSize;

    ssize_t retval = clientReceive(rawmsg, 256);

    memcpy(&msg->address, rawmsg, 1);
    memcpy(&msg->function, rawmsg + 1, 1);
    memcpy(&msg->size, rawmsg + 2, 1);    
    msgSize = static_cast<size_t>(msg->size);

    memcpy(&msg->data, rawmsg + 3, msgSize);
    memcpy(&temp_fh, rawmsg + 3, 2);
    memcpy(&temp_fl, rawmsg + 5, 2);
    memcpy(&temp_i, rawmsg + 7, 2);

    memcpy(temp_f, &temp_fl, 2);
    memcpy(temp_f + 2, &temp_fh, 2);
    memcpy(&tmp, temp_f, 4);        // store in tmp unsigned int variable

    msg->ashur_int_value = bswap_16(temp_i);
    msg->ashur_double_value = static_cast<double>(tmp) * 1e-11;

    return retval;
}

ssize_t ASHURtunnelVICO100::clientSend(const void *msg, size_t msgSize)
{
    ssize_t retval;

    retval = send(socketDescriptor, msg, msgSize, 0);

    return retval;
}

ssize_t ASHURtunnelVICO100::clientReceive(void *msg, size_t msgSize)
{
    ssize_t retval;

    retval = recv(socketDescriptor, msg, msgSize, MSG_DONTWAIT);

    return retval;
}
