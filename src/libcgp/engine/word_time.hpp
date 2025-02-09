#ifndef ENGINE_WORD_TIME_HPP_
#define ENGINE_WORD_TIME_HPP_

#include <libcgp/defines.hpp>

#include <cstdint>
#include <string>

LIBGCP_DECL_START_

class WordTime
{
    // ------------------------------
    // Class internals
    // ------------------------------

    public:
    static constexpr uint64_t kTicksToSecond = 1e+6;
    static constexpr uint64_t kTicksToMinute = 60 * kTicksToSecond;
    static constexpr uint64_t kTicksToHour   = 60 * kTicksToMinute;
    static constexpr uint64_t kTicksToDay    = 24 * kTicksToHour;

    // ------------------------------
    // Object creation
    // ------------------------------

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    void UpdateTime(uint64_t delta_time) noexcept;

    NDSCRD static std::string GetTime() noexcept;

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    /* to ensure timer accuracy */
    uint64_t tick_remainder_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_WORD_TIME_HPP_
