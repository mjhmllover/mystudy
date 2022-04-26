#ifndef BAD_COMP_H
#define BAD_COMP_H

// 用一个类描述出一天的上班状态

class Work
{
public:
    Work();
    ~Work() {}

public:
    void SetHour(int iHour);
    void SetTaskFinished(bool bFinish);

public:
    void WriteProgram();

private:
    int m_iHour;
    bool m_bFinish;
};

#endif