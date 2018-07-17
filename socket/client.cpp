/***********************************************
 * note: string is not built-in type and cannot be a buffer
 
 **********************************************/

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "common.h"

const INT32 cli_domain = AF_INET;
const INT32 cli_type = SOCK_STREAM;
const INT32 cli_protocol = 0;

const INT32 ser_domain = AF_INET;
const INT32 ser_hport = 3306;
const BYTE* ser_hip = (BYTE*)"192.168.174.132";

INT32 main( INT32 argc, BYTE **argv )
{
    /*******create server socket*******/
    INT32 cli_sockfd = socket( cli_domain, cli_type, cli_protocol );
    std::cout << "client sockfd = " << cli_sockfd << std::endl;
    if ( cli_sockfd == -1 )
    {
        return ERROR_CREATE_SOCKET_FAIL;
    }

    /******connet to server*******/
    sockaddr_in ser_addr;
    ser_addr.sin_family = ser_domain;
    ser_addr.sin_port = htons( ser_hport );
    std::cout << "server host port: " << ser_hport << std::endl;
    std::cout << "server net port: " << ser_addr.sin_port << std::endl;
    uint32_t ser_nip = 0;
    INT32 get_pton = inet_pton( ser_domain, ser_hip, &ser_nip );
    std::cout << "server host IP: " << ser_hip << std::endl;
    std::cout << "server net IP: " << ser_nip << std::endl;
    ser_addr.sin_addr.s_addr = ser_nip;
    INT32 get_connect = connect( cli_sockfd, (sockaddr*)&ser_addr, sizeof(ser_addr) );
    std::cout << "connect state: " << get_connect << std::endl;
    if ( get_connect == -1 )
    {
        return ERROR_CONNECT_FAIL;
    }
    
    ssize_t get_recv;
    //do
    //{
        /******send message to server and receive response from server******/
        const UINT8 send_buf[] = {"hello, server!"};
        ssize_t get_send = send( cli_sockfd, &send_buf, sizeof(send_buf), 0 );
        std::cout << "send message to server: " << send_buf;
        std::cout << " ;size of send_buf is " << get_send << std::endl;

        UINT8 recv_buf[15] = {0};
        get_recv = recv( cli_sockfd, &recv_buf, sizeof(recv_buf), 0 );
        std::cout << "receive message from server: " << recv_buf;
        std::cout << " ;size of recv_buf is " << get_recv << std::endl;
    //}
    //while ( !get_recv );

    close(cli_sockfd);

    return OPERATION_SUCCESS;
}