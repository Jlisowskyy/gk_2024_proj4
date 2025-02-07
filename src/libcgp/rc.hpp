#ifndef RC_HPP_
#define RC_HPP_

#include <libcgp/defines.hpp>

#include <array>
#include <cstdlib>

LIBGCP_DECL_START_
enum class Rc {
    kSuccess,
    kUnknownFailure,
    kFailedToLoad,
    kFailedToOpenFile,
    kFileNotFound,
    kDirNotFound,
    kNotADirectory,
    kFailedToCreateDir,
    kNoPermission,
    kFileAlreadyExists,
    kLast,
};

static constexpr std::array kRcDescriptions{
    "Success",        "Unknown failure",     "Failed to load",  "Failed to open file",
    "File not found", "Directory not found", "Not a directory", "Failed to create directory",
    "No permission",  "File already exists",
};

static_assert(kRcDescriptions.size() == static_cast<size_t>(Rc::kLast), "Rc descriptions list is incomplete");

FAST_CALL constexpr const char *GetRcDescription(const Rc code) noexcept
{
    return kRcDescriptions[static_cast<size_t>(code)];
}

static FAST_CALL constexpr bool IsSuccess(const Rc code) noexcept { return code == Rc::kSuccess; }

static FAST_CALL constexpr bool IsFailure(const Rc code) noexcept { return !IsSuccess(code); }

LIBGCP_DECL_END_

#endif  // RC_HPP_
