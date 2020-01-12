#ifndef SIMPLE_FACTORY_MODE_H
#define SIMPLE_FACTORY_MODE_H

// 简单工厂模式：用一个单独的类去构造实例
// 示例：做一个简单的计算器

// 运算类
class Operation
{
public:
    Operation();
    virtual ~Operation();
public: 
    virtual double GetResult();
    void SetNums(double dNumLeft, double dNumRight);

protected:
    double dNumLeft;
    double dNumRight;
};

// 加法类
class OperationAdd : public Operation
{
public:
    double GetResult();
};

// 减法类
class OperationSub : public Operation
{
public:
    double GetResult();    
};

// 乘法类
class OperationMul : public Operation
{
public:
    double GetResult();    
};

// 除法类
class OperationDiv : public Operation
{
public:
    double GetResult();    
};

// 简单运算工厂类
class OperationFactory
{
public:
    static Operation* CreateOperation(char cOperate);
};

#endif