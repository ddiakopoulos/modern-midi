#pragma comment(user, "license")

#ifndef HIGH_RESOLUTION_TIMER_H
#define HIGH_RESOLUTION_TIMER_H

struct IPlatformTimer
{
    virtual ~IPlatformTimer() {};
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual double running_time_ms() const = 0;
    virtual double running_time_s() const = 0;
    virtual double diff_ms() const = 0;
};

#if defined(MM_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class PlatformTimer : public IPlatformTimer
{
    LARGE_INTEGER start_timestamp;
    LARGE_INTEGER stop_timestamp;
    LARGE_INTEGER timer_frequency;
public:
    PlatformTimer()
    {
        start_timestamp.QuadPart = 0;
        stop_timestamp.QuadPart = 0;
    }
    
    virtual ~PlatformTimer() {}
    
    virtual void start() override
    {
        QueryPerformanceFrequency(&timer_frequency);
        QueryPerformanceCounter(&start_timestamp);
    }
    
    virtual void stop() override
    {
        QueryPerformanceCounter(&stop_timestamp);
    }
    
    virtual double running_time_ms() const override
    {
        LARGE_INTEGER tmp;
        QueryPerformanceCounter(&tmp);
        return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
    }
    
    virtual double running_time_s() const override
    {
        LARGE_INTEGER tmp;
        QueryPerformanceCounter(&tmp);
        return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart;
    }
    
    virtual double diff_ms() const override
    {
        return (double)(stop_timestamp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
    }
};

#elif defined(MM_PLATFORM_OSX)
#include <mach/mach_time.h>

class PlatformTimer : public IPlatformTimer
{
    uint64_t start_timestamp;
    uint64_t stop_timestamp;
    uint64_t timer_frequency;
public:
    PlatformTimer()
    {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        timer_frequency = (static_cast<double>(info.numer)) / (static_cast<double>(info.denom));
    }
    
    virtual ~PlatformTimer() {}
    
    virtual void start() override
    {
        start_timestamp = mach_absolute_time();
    }
    
    virtual void stop() override
    {
         stop_timestamp = mach_absolute_time();
    }
    
    virtual double running_time_ms() const override
    {
        uint64_t current_time = mach_absolute_time();
        return (double)(current_time - start_timestamp) * timer_frequency * 1e-6;
    }
    
    virtual double running_time_s() const override
    {
        uint64_t current_time = mach_absolute_time();
        return (double)(current_time - start_timestamp) * timer_frequency * 1e-9;
    }
    
    virtual double diff_ms() const override
    {
        return static_cast<double>(stop_timestamp - start_timestamp) * timer_frequency * 1e-6;
    }
};

#else
    #error Unimplemented timer for desired platform
#endif

#endif