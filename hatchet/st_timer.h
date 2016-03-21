// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: Wed Aug 11 10:38:17 2010
#pragma once
#ifndef _ST_TIMER_H_
#define _ST_TIMER_H_ 1

#include <sys/time.h>

namespace st {
// A very simple timer by gettimeofday, used as follows:
//     Timer t;
//     t.start();
//     ... code piece to time ...
//     t.stop();
//     call t.u_elapsed() or t.m_elapsed() to get the interval
class Timer {
public:
    Timer()
    {
        start_.tv_sec = 0;
        start_.tv_usec = 0;
        stop_.tv_sec = 0;
        stop_.tv_usec = 0;
    }

    // Start this timer
    void start()
    {
        gettimeofday(&start_, NULL);
        stop_ = start_;
    }
    
    // Stop this timer
    void stop() { gettimeofday(&stop_, NULL); }

    // Get the elapse from starting to stoping, in nanoseconds
    long n_elapsed() const { return u_elapsed() * 1000l; }

    // Get the elapse from starting to stoping, in microseconds 
    long u_elapsed() const
    { return (stop_.tv_sec-start_.tv_sec) * 1000000l 
        + (stop_.tv_usec-start_.tv_usec); }

    // Get the elapse from starting to stoping, in milliseconds 
    long m_elapsed() const { return u_elapsed() / 1000l; }
    
    // Get the elapse from starting to stoping, in seconds
    long elapsed() const { return m_elapsed() / 1000l; }

private:
    struct timeval start_;
    struct timeval stop_;
};

// Example:
// {
//     TIME_SCOPE("description of the code", <optional elapse denominator>);
//     ... code to time ...
// }
//
// A line will be printed to console after the scope:
// "description of the code = ...ns"
//
// The default unit is nanosecond, You may change it by using
// TIME_SCOPE_IN_US or TIME_SCOPE_IN_MS or TIME_SCOPE_IN_S
//
// Newline is printed as default, to disable it, add `_NO_NL' posfix

class ScopedPrintTimer {
public:
    // Unit of time for printing
    enum TimeUnit { IN_NS, IN_US, IN_MS, IN_S };

    // Construct this timer and record starting time
    ScopedPrintTimer (TimeUnit tu,               // print in this unit
                      bool new_line,             // print newline?
                      const char* desc_str,      // prefix of printing
                      long div = 1)              // elapse denominator
        : desc_str_(desc_str)
        , div_(0 == div ? 1 : div)
        , tu_(tu)
        , new_line_(new_line)
    {
        gettimeofday(&start_, NULL);
    }

    // Compute and print elapse, and destroy this timer
    ~ScopedPrintTimer ()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        double e_in_us = double((now.tv_sec-start_.tv_sec) * 1000000l
                                + (now.tv_usec-start_.tv_usec));
        switch (tu_) {
        case IN_NS:
            printf("%s %.1fns", desc_str_, e_in_us*1000.0/div_);
            break;
        case IN_US:
            printf("%s %.1fus", desc_str_, e_in_us/div_);
            break;
        case IN_MS:
            printf("%s %.1fms", desc_str_, e_in_us/(div_*1000l));
            break;
        case IN_S:
            printf("%s %.1fs", desc_str_, e_in_us/(div_*1000000l));
            break;
        default:
            printf("Unknown time unit");
        }
        
        if (new_line_) {
            printf ("\n");
        }
    }

private:
    const char* desc_str_;
    long div_;
    struct timeval start_;
    TimeUnit tu_;
    bool new_line_;
};

}  // namespace st

typedef st::Timer NaiveTimer;  // to not break code

using st::ScopedPrintTimer;

#define TIME_SCOPE TIME_SCOPE_IN_NS
#define TIME_SCOPE_NO_NL TIME_SCOPE_IN_NS_NO_NL

#define TIME_SCOPE_IN_NS(...)                                           \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_NS, true, __VA_ARGS__)

#define TIME_SCOPE_IN_US(...)                                           \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_US, true, __VA_ARGS__);      

#define TIME_SCOPE_IN_MS(...)                                           \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_MS, true, __VA_ARGS__);      

#define TIME_SCOPE_IN_S(...)                                            \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_S, true, __VA_ARGS__);       

#define TIME_SCOPE_IN_NS_NO_NL(...)                                     \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_NS, false, __VA_ARGS__);     

#define TIME_SCOPE_IN_US_NO_NL(...)                                     \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_US, false, __VA_ARGS__);     

#define TIME_SCOPE_IN_MS_NO_NL(...)                                     \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_MS, false, __VA_ARGS__);     

#define TIME_SCOPE_IN_S_NO_NL(...)                                      \
    ScopedPrintTimer __stm(ScopedPrintTimer::IN_S, false, __VA_ARGS__);      


#endif  // _ST_TIMER_H_
