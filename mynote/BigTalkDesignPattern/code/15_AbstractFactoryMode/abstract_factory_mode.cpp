#include <iostream>
#include "abstract_factory_mode.h"

// user表

User::User(int iID, string strName)
    : m_strName(strName)
{
    m_iID = iID;
}

int User::GetID()
{
    return m_iID;
}

string User::GetName()
{
    return m_strName;
}

void User::SetID(int iID)
{
    m_iID = iID;
}

void User::SetName(string &strName)
{
    m_strName = strName;
}

void SqlserverUser::Insert(User* ptUser)
{
    cout << "sql: insert into user values(" << ptUser->GetID() << "," \
         << ptUser->GetName() << ");" << endl;
}

User* SqlserverUser::GetUser(int iID)
{
    cout << "sql: select name from user where id = " << iID << ";" << endl;

    User* ptUser = new User(iID, "sql_name");

    return ptUser;
}

void AccessUser::Insert(User* ptUser)
{
    cout << "access: insert into user values(" << ptUser->GetID() << "," \
         << ptUser->GetName() << ");" << endl;
}

User* AccessUser::GetUser(int iID)
{
    cout << "access: select name from user where id = " << iID << ";" << endl;

    User* ptUser = new User(iID, "access_name");

    return ptUser;
}

// Department表

Department::Department(int iID, string strName)
    : m_strName(strName)
{
    m_iID = iID;
}

int Department::GetID()
{
    return m_iID;
}

string Department::GetName()
{
    return m_strName;
}

void Department::SetID(int iID)
{
    m_iID = iID;
}

void Department::SetName(string &strName)
{
    m_strName = strName;
}

void SqlserverDepartment::Insert(Department* ptDepartment)
{
    cout << "sql: insert into Department values(" << ptDepartment->GetID() << "," \
         << ptDepartment->GetName() << ");" << endl;
}

Department* SqlserverDepartment::GetDepartment(int iID)
{
    cout << "sql: select name from Department where id = " << iID << ";" << endl;

    Department* ptDepartment = new Department(iID, "sql_name");

    return ptDepartment;
}

void AccessDepartment::Insert(Department* ptDepartment)
{
    cout << "access: insert into Department values(" << ptDepartment->GetID() << "," \
         << ptDepartment->GetName() << ");" << endl;
}

Department* AccessDepartment::GetDepartment(int iID)
{
    cout << "access: select name from Department where id = " << iID << ";" << endl;

    Department* ptDepartment = new Department(iID, "access_name");
    
    return ptDepartment;
}

// 创建访问表User对象和表Department对象的抽象的工厂接口，可由下述简单工厂替代

IUSER* SqlserverFactory::CreateUser()
{
    return new SqlserverUser();
}

IDepartment* SqlserverFactory::CreateDepartment()
{
    return new SqlserverDepartment();
}

IUSER* AccessFactory::CreateUser()
{
    return new AccessUser();
}

IDepartment* AccessFactory::CreateDepartment()
{
    return new AccessDepartment();
}

// 管理user对象和department对象的创建

IUSER* DataAccess::CreateUser(string strDB)
{
    IUSER* ptIUSER;

    if ("Sqlserver" == strDB) 
    {
        ptIUSER = new SqlserverUser();
    }
    else if ("Access" == strDB)
    {
        ptIUSER = new AccessUser();
    }
    else
    {
        ptIUSER = NULL;
    }

    return ptIUSER;
}

IDepartment* DataAccess::CreateDepartment(string strDB)
{
    IDepartment* ptIDepartment;

    if ("Sqlserver" == strDB) 
    {
        ptIDepartment = new SqlserverDepartment();
    }
    else if ("Access" == strDB)
    {
        ptIDepartment = new AccessDepartment();
    }
    else
    {
        ptIDepartment = NULL;
    }

    return ptIDepartment;
}