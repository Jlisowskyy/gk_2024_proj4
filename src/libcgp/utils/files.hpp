#ifndef UTILS_FILES_HPP_
#define UTILS_FILES_HPP_

#include <libcgp/defines.hpp>

#include <string>

LIBGCP_DECL_START_

NDSCRD bool FileWriteable(const std::string &path) noexcept;

LIBGCP_DECL_END_

#endif  // UTILS_FILES_HPP_
