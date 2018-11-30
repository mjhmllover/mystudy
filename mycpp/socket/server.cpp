#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "common.h"

INT32 main( INT32 argc, BYTE **argv )
{
    /*******create server socket*******/
    INT32 ser_sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    std::cout << "server sockfd = " << ser_sockfd << std::endl;
    if ( ser_sockfd == -1 )
    {
        return ERROR_CREATE_SOCKET_FAIL;
    }

    /******convert little-endian to big-endian******/
    uint16_t hostport = htons( 3306 );

    /******convert string IP to int IP******/
    uint32_t iIP = 0;
    INT32 get_pton = inet_pton( AF_INET, "192.168.174.132", &iIP );
    std::cout << "pton: 192.168.174.132 to " << iIP << std::endl;

    /******bind address to sock******/
    sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = hostport;    
    ser_addr.sin_addr.s_addr = iIP;
    INT32 get_bind = bind( ser_sockfd, (sockaddr*)&ser_addr, sizeof(ser_addr) );
    if ( get_bind == -1 )
    {
        return ERROR_BIND_FAIL;
    }

    /******listen client******/
    INT32 backlog = 4;
    INT32 get_listen = listen( ser_sockfd, backlog );
    if ( get_listen == -1 )
    {
        return ERROR_LISTEN_FAIL;
    }

    /******accept link from client******/
    /*  1. server accepts link from client after three shakehands.
        2. server will be waiting if not accepting request from client when running accept function.
        return: success-a new socket file descriptor, fail-return -1.
    ************************************/
    sockaddr_in cli_addr; //client address
    socklen_t cli_addr_len = sizeof(cli_addr);
    /*cannot write directly &sizeof*/
    INT32 get_accept = accept( ser_sockfd, (sockaddr*)&cli_addr, &cli_addr_len ); 
    if ( get_accept == -1 ) 
    {
        return ERROR_ACCEPT_FAIL;
    }

    ssize_t get_recv;
    //do
    //{
        /******receive message from client******/
        UINT8 recv_buf[15] = {0};
        get_recv = recv( get_accept, &recv_buf, sizeof(recv_buf), 0 );
        std::cout << "receive message from client: " << recv_buf;
        std::cout << " ;size of recv_buf is " << get_recv << std::endl;

        //if ( !get_recv )
        //{
        //    continue;
        //}

        /******send response to client******/
        const UINT8 send_buf[] = {"Hello,client!"};
        ssize_t get_send = send( get_accept, &send_buf, sizeof(send_buf), 0 );
        std::cout << "send message to client: " << send_buf;
        std::cout << " ;size of send_buf is " << get_send << std::endl;
    //}
    //while ( !get_recv );

    close( ser_sockfd ); //include unistd.h
    close( get_accept );

    return OPERATION_SUCCESS;
}