#ifndef SHARELIB_TCPCLIENT_H_
#define SHARELIB_TCPCLIENT_H_

#include<iostream>    
#include<stdio.h> 
#include<string> 
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<netdb.h> 
#include<sharelib/common.h>
SHARELIB_BS;

class TcpClient {
private:
    std::string address_;
    int port_;
    int sock_;
    struct sockaddr_in server_;
    char* buffer;//[102400];
    int receiveBytes;
    uint32_t max_buff_size_;
public:
    TcpClient();
    ~TcpClient();
public:
    const char* GetReceive(size_t& size)const {
        size = receiveBytes -4 ;
        return buffer + 4;
    }
    int GetReceiveBytes(){return receiveBytes;}
    int Conn(const std::string address, const int port);
    int Reconn(const std::string address , const int port);
    int SendData(const char* data, const int size);
    int Receive();
    std::string GetAddress() const {
        return address_;
    }

    int Getport() const {
        return port_;
    }
};
SHARELIB_ES;

#endif //SHARELIB_TCPCLIENT_H_
