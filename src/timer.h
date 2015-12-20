#pragma comment(user, "license")

#ifndef HIGH_RESOLUTION_TIMER_H
#define HIGH_RESOLUTION_TIMER_H

#if defined(MM_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class PlatformTimer
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
    
    void start()
    {
        QueryPerformanceFrequency(&timer_frequency);
        QueryPerformanceCounter(&start_timestamp);
    }
    
    void stop()
    {
        QueryPerformanceCounter(&stop_timestamp);
    }
    
    double running_time_ms() const
    {
        LARGE_INTEGER tmp;
        QueryPerformanceCounter(&tmp);
        return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
    }
    
    double running_time_s() const
    {
        LARGE_INTEGER tmp;
        QueryPerformanceCounter(&tmp);
        return (double) (tmp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart;
    }
    
    double diff_ms() const
    {
        return (double)(stop_timestamp.QuadPart - start_timestamp.QuadPart) / timer_frequency.QuadPart * 1000;
    }
};

#elif defined(MM_PLATFORM_OSX)
#include <mach/mach_time.h>

class PlatformTimer
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
    
    void start()
    {
        start_timestamp = mach_absolute_time();
    }
    
    void stop()
    {
         stop_timestamp = mach_absolute_time();
    }
    
    double running_time_ms() const
    {
        uint64_t current_time = mach_absolute_time();
        return (double)(current_time - start_timestamp) * timer_frequency * 1e-6;
    }
    
    double running_time_s() const
    {
        uint64_t current_time = mach_absolute_time();
        return (double)(current_time - start_timestamp) * timer_frequency * 1e-9;
    }
    
    double diff_ms() const
    {
        return static_cast<double>(stop_timestamp - start_timestamp) * timer_frequency * 1e-6;
    }
};

#else
    #error Unimplemented timer for desired platform
#endif

#endif