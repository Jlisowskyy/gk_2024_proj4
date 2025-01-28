#ifndef LIBGCP_MESH_HPP_
#define LIBGCP_MESH_HPP_

#include <vector>

#include <libcgp/defines.hpp>

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

    struct alignas(32) Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };

    // ------------------------------
    // Object creation
    // ------------------------------

    Mesh() = delete;

    ~Mesh() = default;

    Mesh(std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices, std::vector<Texture> &&textures);

    Mesh(const Mesh &)            = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(Mesh &&) noexcept            = default;
    Mesh &operator=(Mesh &&) noexcept = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Draw(Shader &shader) const;

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    void SetupMesh_();

    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<Vertex> vertices_;
    std::vector<GLuint> indices_;
    std::vector<Texture> textures_;

    GLuint VAO_{};
    GLuint VBO_{};
    GLuint EBO_{};
};

LIBGCP_DECL_END_

#endif  // LIBGCP_MESH_HPP_
