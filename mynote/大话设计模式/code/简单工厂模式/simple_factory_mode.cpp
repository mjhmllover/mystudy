#include "simple_factory_mode.h"
#include <iostream>

Operation::Operation()
: dNumLeft(0), dNumRight(0)
{}

Operation::~Operation()
{}

double Operation::GetResult()
{
    return 0;
}

void Operation::SetNums(double _dNumLeft, double _dNumRight)
{
    dNumLeft  = _dNumLeft;
    dNumRight = _dNumRight;
}

double OperationAdd::GetResult()
{
    return dNumLeft + dNumRight;
}

double OperationSub::GetResult()
{
    return dNumLeft - dNumRight;
}

double OperationMul::GetResult()
{
    return dNumLeft * dNumRight;
}

double OperationDiv::GetResult()
{
    if (0 == dNumRight)
    {
        std::cout << "除数不能为0" << std::endl;
        return 0;
    }

    return dNumLeft / dNumRight;
}

Operation* OperationFactory::CreateOperation(char cOperate)
{
    Operation* oper = NULL;
    switch (cOperate)
    {
        case '+':
            oper = new (std::nothrow) OperationAdd();
            break;
        case '-':
            oper = new (std::nothrow) OperationSub();
            break;
        case '*':
            oper = new (std::nothrow) OperationMul();
            break;
        case '/':
            oper = new (std::nothrow) OperationDiv();
            break;    
        default:
            std::cout << "error operation" << std::endl;
    }

    return oper;
}