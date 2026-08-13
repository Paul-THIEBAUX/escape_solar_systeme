#pragma once
#include <chrono> 

#include "include.h"

using Seconds = float64;

class Clock
{
    using clock = std::chrono::steady_clock;

    clock::time_point m_LastUpdateTime;
    double m_TimeScale;

    Seconds m_UnscaleTime;
    Seconds m_Time;

    Seconds m_DeltaTime;
    Seconds m_UnscaleDeltaTime;

    // FPS
    float32 m_FpsTimer = 0;
    int16   m_FpsCount = 0;
    Seconds m_RawDeltaTime;
public:
    Clock();

    ~Clock() = default;

    void Restart(float64 t = 0.0);
    void Update();

    [[nodiscard]] Seconds GetDeltaTime() const;
    [[nodiscard]] Seconds GetDeltaTimeUnscaled() const;

    [[nodiscard]] Seconds GetTime() const;
    [[nodiscard]] Seconds GetTimeUnscaled() const;

    [[nodiscard]] int16 GetFramePerSecond() const;

    [[nodiscard]] float64 GetTimeScale() const;
    void SetTimeScale(float64 scale);

    clock::time_point GetLastUpdateTime() const;
    Seconds GetRawDT() const;

    void ClockSleep(Seconds time);
    void SmartSleep(Seconds time);
};