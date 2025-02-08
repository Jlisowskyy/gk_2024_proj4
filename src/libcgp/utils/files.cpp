#include <libcgp/utils/files.hpp>

#include <filesystem>
#include <string>

bool LibGcp::FileWriteable(const std::string &path) noexcept
{
    std::error_code ec;

    const std::filesystem::perms perms = std::filesystem::status(path, ec).permissions();
    if (ec) {
        return false;
    }

    return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
}

bool LibGcp::FileReadable(const std::string &path) noexcept
{
    std::error_code ec;

    const std::filesystem::perms perms = std::filesystem::status(path, ec).permissions();

    if (ec) {
        return false;
    }

    return (perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none;
}
