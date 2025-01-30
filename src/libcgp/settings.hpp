#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <libcgp/defines.hpp>

#include <cinttypes>

LIBGCP_DECL_START_

enum class CameraType : std::uint8_t {
    kStatic,
    kFollow,
    kFree,
    kFirstPerson,
    kThirdPerson,
    kLast,
};

LIBGCP_DECL_END_

#endif  // SETTINGS_HPP_
