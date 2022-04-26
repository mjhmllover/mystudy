#ifndef ABSTRACT_FACTORY_MODE_H
#define ABSTRACT_FACTORY_MODE_H

#include <string>

using namespace std;

// 定义user表，只有ID和Name两个字段

class User
{
public:
    User(int iID, string strName);
    ~User() {}

public:
    int GetID();
    string GetName();

    void SetID(int iID);
    void SetName(string &strName);

private:
    int m_iID;
    string m_strName;
};

// 定义访问表User的接口

class IUSER
{
public:
    virtual ~IUSER() {}

public:
    virtual void Insert(User* ptUser) {}

    virtual User* GetUser(int iID) 
    {
        return NULL;
    }
};

class SqlserverUser : public IUSER
{
public:
    void Insert(User* ptUser);
    User* GetUser(int iID);    
};

class AccessUser : public IUSER
{
public:
    void Insert(User* ptUser);
    User* GetUser(int iID);    
};

// 定义Department表，只有ID和DeptName两个字段

class Department
{
public:
    Department(int iID, string strName);
    ~Department() {}

public:
    int GetID();
    string GetName();

    void SetID(int iID);
    void SetName(string &strName);

private:
    int m_iID;
    string m_strName;
};

// 定义访问表Department的接口

class IDepartment
{
public:
    virtual ~IDepartment() {}

public:
    virtual void Insert(Department* ptDepartment) {}
    
    virtual Department* GetDepartment(int iID)
    {
        return NULL;
    }
};

class SqlserverDepartment : public IDepartment
{
public:
    void Insert(Department* ptDepartment);
    Department* GetDepartment(int iID);    
};

class AccessDepartment : public IDepartment
{
public:
    void Insert(Department* ptDepartment);
    Department* GetDepartment(int iID);    
};

// 定义创建访问表User对象和表Department对象的抽象的工厂接口
class IFactory
{
public:
    virtual ~IFactory() {}

    virtual IUSER* CreateUser() 
    {
        return NULL;
    }

    virtual IDepartment* CreateDepartment()
    {
        return NULL;
    }
};

class SqlserverFactory : public IFactory
{
public:
    IUSER* CreateUser();
    IDepartment* CreateDepartment();
};

class AccessFactory : public IFactory
{
public:
    IUSER* CreateUser();
    IDepartment* CreateDepartment();
};

// 简单工厂类，管理对表User对象和表Department对象的创建
class DataAccess
{
public:
    static IUSER* CreateUser(string strDB = "Sqlserver");
    static IDepartment* CreateDepartment(string strDB = "Sqlserver");
};

#endif