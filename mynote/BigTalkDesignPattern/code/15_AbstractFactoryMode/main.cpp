#include <iostream>

#include "abstract_factory_mode.h"

int main()
{
    // two tables: user and department
    User* ptUser = new User(1, "zte");

    IUSER* ptIUSER = DataAccess::CreateUser("Access");
    if (NULL == ptIUSER)
    {
        cout << "ptIUSER is NULL" << endl;
        
        delete ptUser;
        ptUser = NULL;

        return -1;
    }

    ptIUSER->Insert(ptUser);
    User* ptTmpUser = ptIUSER->GetUser(2);

    Department* ptDepartment = new Department(1, "HR");

    IDepartment* ptIDepartment = DataAccess::CreateDepartment("Access");
    if (NULL == ptIDepartment)
    {
        cout << "ptIDepartment is NULL" << endl;

        delete ptUser;
        ptUser = NULL;

        delete ptIUSER;
        ptIUSER = NULL;

        delete ptTmpUser;
        ptTmpUser = NULL;

        delete ptDepartment;
        ptDepartment = NULL;

        return -1;
    }

    ptIDepartment->Insert(ptDepartment);
    Department* ptTmpDepartment = ptIDepartment->GetDepartment(2);

    delete ptUser;
    ptUser = NULL;

    delete ptIUSER;
    ptIUSER = NULL;

    delete ptTmpUser;
    ptTmpUser = NULL;

    delete ptDepartment;
    ptDepartment = NULL;

    delete ptIDepartment;
    ptIDepartment = NULL;

    delete ptTmpDepartment;
    ptTmpDepartment = NULL;

    return 0;
}