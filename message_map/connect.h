#include "base.h"

class Connect:public Base
{
public:
    Connect(){};
    ~Connect(){};
    UINT8* ConnectReq(UINT8* msg);
    UINT8* ConnectRsp(UINT8* msg);
    DECLARE_MESSAGE_MAP(Connect)
};
