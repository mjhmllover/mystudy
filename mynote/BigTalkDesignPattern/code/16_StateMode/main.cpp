#include <iostream>

#include "state_mode.h"

int main()
{
    Work* ptWork = new Work();
    
    ptWork->SetHour(9);
    ptWork->WriteProgram();

    ptWork->SetHour(10);
    ptWork->WriteProgram();

    ptWork->SetHour(12);
    ptWork->WriteProgram();

    ptWork->SetHour(13);
    ptWork->WriteProgram();

    ptWork->SetHour(14);
    ptWork->WriteProgram();

    ptWork->SetHour(17);
    ptWork->SetTaskFinished(true);
    ptWork->WriteProgram();

    ptWork->SetHour(19);
    ptWork->WriteProgram();
    
    ptWork->SetHour(22);
    ptWork->WriteProgram();

    delete ptWork;
    ptWork = NULL;

    return 0;
}