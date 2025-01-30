#include <libcgp/primitives/mesh.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/texture.hpp>
#include <libcgp/utils/macros.hpp>

#include <glad/gl.h>

#include <array>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

LibGcp::Mesh::Mesh(
    std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices, std::vector<std::shared_ptr<Texture>> &&textures
)
    : vertices_(std::move(vertices)), indices_(std::move(indices)), textures_(std::move(textures))
{
    R_ASSERT(vertices_.size() > 0);
    R_ASSERT(indices_.size() > 0);

    SetupMesh_();
}

void LibGcp::Mesh::Draw(Shader &shader) const
{
    static constexpr uint8_t kMaxTextures = 16;

    std::array<std::string, static_cast<size_t>(Texture::Type::kLast)> texture_names = {
        "texture_diffuse00",
        "texture_specular00",
    };  // leave space for numbers

    std::array counters = {static_cast<uint8_t>(0), static_cast<uint8_t>(0)};

    for (size_t idx = 0; idx < textures_.size(); ++idx) {
        const size_t type_idx = static_cast<size_t>(textures_[idx]->GetType());
        const uint8_t counter = ++counters[type_idx];
        std::string &name     = texture_names[type_idx];

        name[name.size() - 2] = '0' + counter / 10;
        name[name.size() - 1] = '0' + counter % 10;

        shader.SetGLfloatUnsafe(name.c_str(), static_cast<GLfloat>(idx));
        textures_[idx]->Bind(idx);

        if (idx >= kMaxTextures) {
            break;
        }
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void LibGcp::Mesh::SetupMesh_()
{
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER, static_cast<ssize_t>(vertices_.size() * sizeof(Vertex)), vertices_.data(), GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, static_cast<ssize_t>(indices_.size() * sizeof(GLuint)), indices_.data(), GL_STATIC_DRAW
    );

    /* vertices */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    /* normals */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));

    /* texture coordinates */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tex_coords))
    );

    glBindVertexArray(0);
}
