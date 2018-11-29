#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "base.h"
#include "connect.h"
#include "common.h"
#include "event.h"

Base base;
Base* ptBase = new Connect();

int main(int argc, char **argv)
{
    UINT8 msg[] = {"this is a test message!"};
    std::cout << base.DispatchMsg(CONNECT_REQ, msg) << std::endl;
    std::cout << base.DispatchMsg(CONNECT_RSP, msg) << std::endl;
    std::cout << ptBase->DispatchMsg(CONNECT_REQ, msg) << std::endl;
    std::cout << ptBase->DispatchMsg(CONNECT_RSP, msg) << std::endl;
    delete ptBase;
    return 0;
}
