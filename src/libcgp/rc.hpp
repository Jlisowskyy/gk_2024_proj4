#ifndef RC_HPP_
#define RC_HPP_

#include <libcgp/defines.hpp>

LIBGCP_DECL_START_
enum class Rc {
    kSuccess,
    kUnknownFailure,
    kFailedToLoad,
    kLast,
};

static FAST_CALL constexpr bool IsSuccess(const Rc code) noexcept { return code == Rc::kSuccess; }

LIBGCP_DECL_END_

#endif  // RC_HPP_
