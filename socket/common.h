#ifndef COMMON_H
#define COMMON_H

typedef char BYTE;

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef signed long INT64;

typedef unsigned char UINT8;
typedef unsigned short UINT16; 
typedef unsigned int UINT32;
typedef unsigned long UINT64; 

const INT32 OPERATION_SUCCESS = 0;
const INT32 ERROR_CREATE_SOCKET_FAIL = 1;
const INT32 ERROR_BIND_FAIL = 2;
const INT32 ERROR_LISTEN_FAIL = 3;
const INT32 ERROR_ACCEPT_FAIL = 4;
const INT32 ERROR_CONNECT_FAIL = 5;

#endif
