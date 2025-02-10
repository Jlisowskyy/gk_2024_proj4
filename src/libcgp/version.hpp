#ifndef VERSION_HPP_
#define VERSION_HPP_

#include <libcgp/defines.hpp>

#include <cinttypes>

LIBGCP_DECL_START_
enum class Version : std::uint16_t {
    V0_1_0,
    kLast,
};

enum class SceneVersion : std::uint16_t {
    V0_1_0,
    V0_1_1,  // Added support for lights
    kLast,
};

enum class TextureVersion : std::uint16_t {
    V0_1_0,
    kLast,
};

enum class ModelVersion : std::uint16_t {
    V0_1_0,
    kLast,
};

static constexpr auto kGlobalVersion     = Version::V0_1_0;
static constexpr auto kMinSceneVersion   = SceneVersion::V0_1_1;
static constexpr auto kMinTextureVersion = TextureVersion::V0_1_0;
static constexpr auto kMinModelVersion   = ModelVersion::V0_1_0;
static constexpr auto kSceneVersion      = SceneVersion::V0_1_1;
static constexpr auto kTextureVersion    = TextureVersion::V0_1_0;
static constexpr auto kModelVersion      = ModelVersion::V0_1_0;

LIBGCP_DECL_END_

#endif  // VERSION_HPP_
