#ifndef STATE_MODE_H
#define STATE_MODE_H

// 用一个类描述出一天的上班状态

class State;
class Work
{
public:
    Work();
    ~Work();

public:
    void SetHour(int iHour);
    void SetTaskFinished(bool bFinish);

    int GetHour();
    bool GetTaskFinished();

public:
    void SetState(State* ptState);
    void WriteProgram();

private:
    void Init();

private:
    int m_iHour;
    bool m_bFinish;

    State* m_ptState;
};

// 用状态类定义状态的转换

class State
{
public:
    State() {}
    virtual ~State() {}

public:
    virtual void WriteProgram(Work* ptWork) {}
};

class ForenoonState : public State
{
public:
    ForenoonState();
    ~ForenoonState();

public:
    void WriteProgram(Work* ptWork);

private:
    State* m_ptState;
};

class NoonState : public State
{
public:
    NoonState();
    ~NoonState();

public:
    void WriteProgram(Work* ptWork);

private:
    State* m_ptState;
};

#endif