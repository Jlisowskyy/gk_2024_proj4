#include <libcgp/primitives/texture.hpp>
#include <libcgp/utils/macros.hpp>

#include <glad/gl.h>

#include <cstdlib>
#include <string>
#include <utility>

LibGcp::Texture::Texture(
    const unsigned char *texture_data, const int width, const int height, const int channels, const Type type
) noexcept
    : type_(type)
{
    R_ASSERT(channels == 3 || channels == 4);

    GLuint texture_id{};

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    texture_id_ = texture_id;
}

LibGcp::Texture::~Texture() noexcept
{
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}
