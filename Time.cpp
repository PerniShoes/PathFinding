#include "Time.h"

Time::Time(float timeGoal)
    :m_IsPaused{false}
    ,m_IsDone{false}
    ,m_AccumulatedTime{0.0f}
    ,m_TimeGoal{timeGoal}
{
    if (m_TimeGoal < 0.0f)
    {
        m_TimeGoal = 0.0f;
    }
    else if (m_TimeGoal > 0.0f)
    {
        m_IsPaused = true;
    }
}

void Time::Update(float elapsedTime)
{
    if (m_IsPaused == true)
        return;

    m_AccumulatedTime += elapsedTime;
    if (m_AccumulatedTime >= m_TimeGoal 
        && m_TimeGoal != 0.0f)
    {
        m_IsDone = true;
        m_IsPaused = true;
    }

}

void Time::Start()
{
    m_IsPaused = false;
}

void Time::Pause()
{
    m_IsPaused = true;
}

void Time::Restart() 
{
    m_AccumulatedTime = 0.0f;
    m_IsDone = false;
}

void Time::RestartAndPause()
{
    Pause();
    Restart();
}

float Time::GetTime() const
{
    return m_AccumulatedTime;
}

float Time::GetTimerPercentage() const
{
    if (m_TimeGoal == 0.0f)
        return 0.0f;

    if (m_IsDone == true)
        return 1.0f;
   
    return m_AccumulatedTime / m_TimeGoal;
}

void Time::SetGoal(float timeGoal)
{
    if (timeGoal < 0.0f)
    {
        timeGoal = 0.0f;
    }
    m_TimeGoal = timeGoal;
}

bool Time::IsDone() const
{
    return m_IsDone;
}