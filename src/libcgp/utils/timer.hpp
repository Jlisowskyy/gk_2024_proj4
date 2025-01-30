#ifndef UTILS_TIMER_HPP_
#define UTILS_TIMER_HPP_

#include <libcgp/defines.hpp>

#include <chrono>
#include <unordered_map>

LIBGCP_DECL_START_
class Timer
{
#ifdef USE_TIMERS_
    static constexpr bool kUseTimers = true;
#else
    static constexpr bool kUseTimers = false;
#endif

    public:
    // ------------------------------
    // Class interaction
    // ------------------------------

    WRAP_CALL void Start(const int desc)
    {
        if constexpr (kUseTimers) {
            Start_(desc);
        }
    }

    WRAP_CALL long Stop(const int desc)
    {
        if constexpr (kUseTimers) {
            return Stop_(desc);
        }
    }

    WRAP_CALL void StopAndPrint(const int desc)
    {
        if constexpr (kUseTimers) {
            StopAndPrint_(desc);
        }
    }

    // ------------------------------
    // Class implementation
    // ------------------------------

    protected:
    void Start_(const int desc)
    {
        R_ASSERT(!start_times_.contains(desc));

        start_times_[desc] = std::chrono::high_resolution_clock::now();
    }

    long Stop_(const int desc)
    {
        R_ASSERT(start_times_.contains(desc));

        const auto end_time   = std::chrono::high_resolution_clock::now();
        const auto start_time = start_times_[desc];
        const auto duration   = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        start_times_.erase(desc);

        return duration;
    }

    void StopAndPrint_(const int desc)
    {
        const auto duration = Stop_(desc);
        std::cout << "Timer " << desc << " took " << duration << " microseconds" << std::endl;
    }

    // ------------------------------
    // Class fields
    // ------------------------------

    std::unordered_map<int, decltype(std::chrono::high_resolution_clock::now())> start_times_{};
};

LIBGCP_DECL_END_

#endif  // UTILS_TIMER_HPP_
