#include "connect.h"
#include "base.h"
#include "event.h"

BEGIN_MESSAGE_MAP(Connect, Base)
    ON_MESSAGE_MAP(CONNECT_REQ, &Connect::ConnectReq)
    ON_MESSAGE_MAP(CONNECT_RSP, &Connect::ConnectRsp)
END_MESSAGE_MAP

UINT8* Connect::ConnectReq(UINT8* msg)
{
    return msg;
}

UINT8* Connect::ConnectRsp(UINT8* msg)
{
    return msg;
}
