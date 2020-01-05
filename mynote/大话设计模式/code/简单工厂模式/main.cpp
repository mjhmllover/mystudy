#include "simple_factory_mode.h"
#include <iostream>

int main()
{
    // 计算1 + 2 = 3
    Operation* oper = OperationFactory::CreateOperation('+');
    oper->SetNums(1, 2);
    double dRes = oper->GetResult();
    std::cout << "result = " << dRes << std::endl;

    delete oper;
    oper = NULL;

    return 0;
}