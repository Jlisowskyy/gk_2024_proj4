#include <libcgp/texture.hpp>
#include <libcgp/utils/macros.hpp>

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdlib>
#include <string>

LibGcp::Texture::Texture(
    const unsigned char *texture_data, const int width, const int height, const int channels, const Type type
) noexcept
    : type_(type)
{
    R_ASSERT(channels == 3 || channels == 4);
    R_ASSERT(type != Type::kLast);

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

LibGcp::Texture LibGcp::MakeTextureFromData(
    const unsigned char *texture_data, const int width, const int height, int channels, const Texture::Type type
) noexcept
{
    return {texture_data, width, height, channels, type};
}

LibGcp::Texture LibGcp::MakeTextureFromFile(const char *file_path, const Texture::Type type) noexcept
{
    int width{};
    int height{};
    int channels{};

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_path, &width, &height, &channels, 0);
    VALIDATE_STATE(data != nullptr, "Failed to load texture");

    Texture texture{data, width, height, channels, type};
    stbi_image_free(data);

    return texture;
}

// LibGcp::Texture LibGcp::MakeTextureFromImage(const char *image_name, const Texture::Type type) noexcept
// {
//     R_ASSERT(StaticImages::g_KnownImages.contains(std::string(image_name)));
//
//     int width{};
//     int height{};
//     int channels{};
//     const auto &image       = StaticImages::g_KnownImages.at(std::string(image_name));
//     const size_t image_size = StaticImages::g_ImageSizes.at(std::string(image_name));
//
//     stbi_set_flip_vertically_on_load(true);
//     unsigned char *data = stbi_load_from_memory(image, static_cast<int>(image_size), &width, &height, &channels, 0);
//     R_ASSERT(data != nullptr);
//
//     Texture texture{data, width, height, channels, type};
//     stbi_image_free(data);
//
//     return texture;
// }
