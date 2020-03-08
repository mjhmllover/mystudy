#include <iostream>

using namespace std;

#include "state_mode.h"

Work::Work()
{
    m_iHour = 0;
    m_bFinish = 0;
    Init();
}

Work::~Work()
{
    if (NULL != m_ptState)
    {
        delete m_ptState;
        m_ptState = NULL;
    } 
}

void Work::SetHour(int iHour)
{
    m_iHour = iHour;
}

void Work::SetTaskFinished(bool bFinish)
{
    m_bFinish = bFinish;
}

int Work::GetHour()
{
    return m_iHour;
}

bool Work::GetTaskFinished()
{
    return m_bFinish;
}

void Work::SetState(State* ptState)
{
    m_ptState = ptState;
}

// 方法过长是坏味道
void Work::WriteProgram()
{
    m_ptState->WriteProgram(this);
}

void Work::Init()
{
    m_ptState = new ForenoonState();
}

ForenoonState::ForenoonState()
{
    m_ptState = NULL;
}

ForenoonState::~ForenoonState()
{
    if (NULL != m_ptState)
    {
        delete m_ptState;
        m_ptState = NULL;
    }
}

void ForenoonState::WriteProgram(Work* ptWork)
{
    if (ptWork->GetHour() < 12)
    {
        cout << "当前时间：" << ptWork->GetHour() << " 点 上午工作，精神百倍" << endl;
    }
    else
    {
        m_ptState = new NoonState();
        ptWork->SetState(m_ptState);
        ptWork->WriteProgram();
    } 
}

NoonState::NoonState()
{
    m_ptState = NULL;
}

NoonState::~NoonState()
{
    if (NULL != m_ptState)
    {
        delete m_ptState;
        m_ptState = NULL;
    }
}

void NoonState::WriteProgram(Work* ptWork)
{
    if (ptWork->GetHour() < 13)
    {
        cout << "当前时间：" << ptWork->GetHour() << " 点 饿了，午饭；犯困，午休。" << endl;
    }
    else
    {
        //State* ptState = new NoonState();
        //ptWork->SetState(ptState);
        //ptWork->WriteProgram();
        cout << "go out" << endl;
    } 
}