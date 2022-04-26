#include "simple_factory_mode.h"
#include <iostream>

void DoAdd()
{
    // 计算1 + 2 = 3
    Operation* oper = OperationFactory::CreateOperation('+');
    oper->SetNums(1, 2);
    double dRes = oper->GetResult();
    std::cout << "result = " << dRes << std::endl;

    // 如果不释放new的对象，程序退出也不会调用对象的析构函数
    delete oper;
    oper = NULL;
}

int main()
{
    DoAdd();

    return 0;
}