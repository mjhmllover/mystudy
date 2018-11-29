#include "base.h"

class Connect:public Base
{
public:
    Connect(){};
    ~Connect(){};
    INT16 ConnectReq(UINT8* msg);
    INT16 ConnectRsp(UINT8* msg);
    DECLARE_MESSAGE_MAP(Connect)
};
