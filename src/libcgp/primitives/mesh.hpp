#ifndef LIBGCP_MESH_HPP_
#define LIBGCP_MESH_HPP_

#include <memory>
#include <vector>

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

#include <glad/gl.h>
#include <glm/glm.hpp>

LIBGCP_DECL_START_
/* Forward declarations */
class Shader;
class Texture;

// ------------------------------
// Mesh class
// ------------------------------

class Mesh
{
    public:
    // ------------------------------
    // Inner types
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    Mesh() = delete;

    ~Mesh() = default;

    Mesh(
        std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices, std::vector<std::shared_ptr<Texture>> &&textures
    );

    Mesh(const Mesh &)            = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(Mesh &&) noexcept            = default;
    Mesh &operator=(Mesh &&) noexcept = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Draw(Shader &shader) const;

    NDSCRD double &GetOpacity() noexcept { return opacity_; }
    NDSCRD double &GetShininess() noexcept { return shininess_; }

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    void SetupMesh_();

    // ------------------------------
    // Class fields
    // ------------------------------

    double opacity_{1.0};
    double shininess_{32.0};

    std::vector<Vertex> vertices_;
    std::vector<GLuint> indices_;
    std::vector<std::shared_ptr<Texture>> textures_;

    GLuint VAO_{};
    GLuint VBO_{};
    GLuint EBO_{};
};

LIBGCP_DECL_END_

#endif  // LIBGCP_MESH_HPP_
