#include <libcgp/primitives/texture.hpp>
#include <libcgp/utils/macros.hpp>

#include <glad/gl.h>

#include <array>
#include <cstdlib>
#include <string>
#include <utility>

LibGcp::Texture::Texture(
    const unsigned char *texture_data, const int width, const int height, const int channels, const Type type
) noexcept
    : type_(type)
{
    R_ASSERT(channels == 3 || channels == 4 || channels == 2 || channels == 1);

    static constexpr std::array kDescTable = {
        0, GL_RED, GL_RG, GL_RGB, GL_RGBA,
    };
    const auto format = kDescTable[channels];

    GLuint texture_id{};

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, channels == 4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, channels == 4 ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture_id_ = texture_id;
}

LibGcp::Texture::~Texture() noexcept
{
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}
