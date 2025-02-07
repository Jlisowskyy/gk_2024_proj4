#ifndef UTILS_FILES_HPP_
#define UTILS_FILES_HPP_

#include <libcgp/defines.hpp>

#include <filesystem>
#include <string>

LIBGCP_DECL_START_

NDSCRD bool FileWriteable(const std::string& path) noexcept;

NDSCRD FAST_CALL std::string GetDirFromFile(const std::string& file)
{
    return std::filesystem::path(file).parent_path().string();
}

NDSCRD FAST_CALL std::string GetFileName(const std::string& file)
{
    return std::filesystem::path(file).filename().string();
}

LIBGCP_DECL_END_

#endif  // UTILS_FILES_HPP_
