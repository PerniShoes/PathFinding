#pragma once

class Time final
{

public:

    // Set time goal to 0.0f to have a general time tracker
    // Set above 0.0f to have a timer
    // Timers have to be ->Start()ed
    // Accepts only positive values
    Time(float timeGoal);
    Time(const Time& time) = delete;
    Time& operator=(const Time& time) = delete;
    Time(Time&& time) = delete;
    Time& operator=(Time&& time) = delete;

    ~Time() = default;

    void Update(float elaspedTime);
    
    void Start();
    void Pause();
    void Restart();
    void RestartAndPause();

    float GetTime() const;
    float GetTimerPercentage() const;
    bool IsDone() const;
    // Accepts only positive values
    void SetGoal(float timeGoal);

private:

    bool m_IsPaused;
    bool m_IsDone;

    float m_AccumulatedTime;
    float m_TimeGoal;
    
};

