/**
 * @file timer.hpp
 * @brief Chrono wrapper to basic types
 */

#pragma once

#include <chrono>

using ns_t      = int64_t;
using us_t      = int64_t;
using ms_t      = int64_t;
using sec_t     = float;

using steady_clock = std::chrono::steady_clock;

/*
 * ns_t to ms_t conversion
 */
inline ms_t ns_to_ms (ns_t ns)
{
    return (ms_t) (ns / 1000000);
}

/*
 * sec_t to us_t conversion
 */
inline us_t sec_to_us (sec_t ts)
{
    return (us_t) (ts * 1000000);
}

/**
 * Get current time in nanoseconds
 */
inline ns_t get_time_ns ()
{
    auto now = steady_clock::now ();
    return ns_t {std::chrono::duration_cast<std::chrono::nanoseconds> (
                    now.time_since_epoch ()).count ()};
}

/**
 * Get current time in milliseconds
 */
inline ms_t get_time_ms ()
{
    return ns_to_ms (get_time_ns ());
}

/**
 * Convert absolute milliseconds since epoch to timepoint
 */
inline std::chrono::time_point<steady_clock> ms_to_tp (ms_t ms)
{
    return std::chrono::time_point<std::chrono::steady_clock,
           std::chrono::milliseconds> (std::chrono::milliseconds (ms));
}

/**
 * Stopwatch class
 */
class Stopwatch
{
private:
    ms_t last_time;
    ms_t total_time;

    enum StopwatchState
    {
        STARTED,
        PAUSED,
        NONE
    };

    StopwatchState state;

public:
    /**
     * Starts the stopwatch
     */
    void start ()
    {
        // Do nothing if already running
        if (state == StopwatchState::STARTED)
            return;

        state = StopwatchState::STARTED;
        last_time = get_time_ms ();
    }

    /**
     * Pauses the stopwatch and returns the running time since last start call.
     */
    ms_t pause ()
    {
        state = StopwatchState::PAUSED;
        ms_t interval = get_time_ms () - last_time;

        total_time += interval;

        return interval;
    }

    void reset ()
    {
        state = StopwatchState::NONE;
        total_time = 0;
    }

    /**
     * Returns total time between starts and pauses.
     * If currently running (start called, not paused), adds cur running time.
     */
    ms_t read ()
    {
        switch (state)
        {
            case StopwatchState::NONE:
                return ms_t {0};
            case StopwatchState::PAUSED:
                return total_time;
            case StopwatchState::STARTED:
                return total_time + (get_time_ms () - last_time);
        }

        return ms_t {0};
    }
};