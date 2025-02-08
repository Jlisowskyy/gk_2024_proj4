#ifndef LIBGCP_DEFINES_HPP_
#define LIBGCP_DEFINES_HPP_

#include <iostream>

#ifdef __GNUC__
#define FORCE_INLINE_ __attribute__((always_inline))
#elif defined(_MSC_VER)
#define FORCE_INLINE_ __forceinline
#elif defined(__clang__)
#define FORCE_INLINE_ __attribute__((always_inline))
#else
#warning "Unknown compile, force inline not supported"
#define FORCE_INLINE_
#endif

#define FAST_CALL inline FORCE_INLINE_
#define WRAP_CALL inline FORCE_INLINE_

// LOCAL_FAST_CALL and LOCAL_WRAP_CALL are used for functions that are only used in single translation unit
#define L_FAST_CALL static FORCE_INLINE_
#define L_WRAP_CALL static FORCE_INLINE_

#define LIBGCP_DECL_START_ \
    namespace LibGcp       \
    {
#define LIBGCP_DECL_END_ }

#define NDSCRD [[nodiscard]]
#define UNUSED [[maybe_unused]]

#ifdef USE_TRACES_
#define TRACE(stream) std::cout << stream << std::endl
#else
#define TRACE(stream)
#endif  // USE_TRACES_

#endif  // LIBGCP_DEFINES_HPP_
