#ifndef LIBGCP_TEXTURE_HPP_
#define LIBGCP_TEXTURE_HPP_

#include <glad/gl.h>

#include <cstdint>

#include <libcgp/defines.hpp>

LIBGCP_DECL_START_
// ------------------------------
// Texture class
// ------------------------------

class Texture
{
    public:
    // ------------------------------
    // Inner types
    // ------------------------------

    enum class Type : std::uint8_t {
        kDiffuse,
        kSpecular,
        kLast,
    };

    // ------------------------------
    // Object creation
    // ------------------------------

    Texture(const unsigned char *texture_data, int width, int height, int channels, Type type) noexcept;

    ~Texture() noexcept;

    /* prohibit copying */
    Texture(const Texture &) = delete;

    Texture &operator=(const Texture &) = delete;

    /* allow move operations as ownership is unique */
    Texture(Texture &&) noexcept = default;

    Texture &operator=(Texture &&) noexcept = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    NDSCRD FAST_CALL GLuint GetTextureId() const noexcept { return texture_id_; }

    NDSCRD FAST_CALL Type GetType() const noexcept { return type_; }

    FAST_CALL void Bind(const int texture_unit) const noexcept
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    GLuint texture_id_{};
    Type type_{};
};

// ------------------------------
// Factories
// ------------------------------

Texture MakeTextureFromData(const unsigned char *texture_data, int width, int height, int channels) noexcept;

Texture MakeTextureFromFile(const char *file_path) noexcept;

Texture MakeTextureFromImage(const char *image_name) noexcept;

LIBGCP_DECL_END_

#endif  // LIBGCP_TEXTURE_HPP_
