#include <iostream>

using namespace std;

#include "bad_comp.h"

Work::Work()
{
    m_iHour = 0;
    m_bFinish = 0;
}

void Work::SetHour(int iHour)
{
    m_iHour = iHour;
}

void Work::SetTaskFinished(bool bFinish)
{
    m_bFinish = bFinish;
}

// 方法过长是坏味道
void Work::WriteProgram()
{
    if (m_iHour < 12)
    {
        cout << "当前时间：" << m_iHour << " 点 上午工作，精神百倍" << endl;
    }
    else if (m_iHour < 13)
    {
        cout << "当前时间：" << m_iHour << " 点 饿了，午饭；犯困，午休。" << endl;
    }
    else if (m_iHour < 17)
    {
        cout << "当前时间：" << m_iHour << " 点 下午状态还不错，继续努力" << endl;
    }
    else
    {
        if (m_bFinish)
        {
            cout << "当前时间：" << m_iHour << " 点 下班回家了" << endl;
        }
        else
        {
            if (m_iHour < 21)
            {
                cout << "当前时间：" << m_iHour << " 点 加班咯，疲累之极" << endl;
            }
            else
            {
                cout << "当前时间：" << m_iHour << " 点 不行了，要睡着了。" << endl;
            }
            
        }
    }
}