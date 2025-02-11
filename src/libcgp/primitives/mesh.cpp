#include <libcgp/primitives/mesh.hpp>
#include <libcgp/primitives/quad.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/texture.hpp>
#include <libcgp/utils/macros.hpp>

#include <glad/gl.h>

#include <array>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

LibGcp::Quad::~Quad() { Destroy(); }

void LibGcp::Quad::Destroy()
{
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
}

void LibGcp::Quad::Init()
{
    static constexpr float kQuadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // setup plane VAO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), &kQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
}

void LibGcp::Quad::Draw() const
{
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

LibGcp::Mesh::Mesh(
    std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices, std::vector<std::shared_ptr<Texture> > &&textures
)
    : vertices_(std::move(vertices)), indices_(std::move(indices)), textures_(std::move(textures))
{
    R_ASSERT(vertices_.size() > 0);
    R_ASSERT(indices_.size() > 0);

    SetupMesh_();
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

    /* tangent */
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tangent))
    );

    glBindVertexArray(0);
}

void LibGcp::Mesh::BindMaterial_(Shader &shader) const noexcept
{
    static constexpr uint8_t kMaxTextures = 16;

    std::array<std::string, static_cast<size_t>(Texture::Type::kLast)> texture_names = {
        "un_material.texture_diffuse00",
        "un_material.texture_specular00",
        "un_material.texture_normal00",
    };  // leave space for numbers

    std::array<uint8_t, static_cast<size_t>(Texture::Type::kLast)> counters{};

    for (size_t idx = 0; idx < textures_.size(); ++idx) {
        if (idx >= kMaxTextures) {
            break;
        }

        const size_t type_idx = static_cast<size_t>(textures_[idx]->GetType());
        const uint8_t counter = ++counters[type_idx];
        std::string &name     = texture_names[type_idx];

        name[name.size() - 2] = '0' + counter / 10;
        name[name.size() - 1] = '0' + counter % 10;

        shader.SetGLint(name.c_str(), idx);
        textures_[idx]->Bind(idx);
    }
    glActiveTexture(GL_TEXTURE0);

    // shader.SetGLfloat("un_material.shininess", static_cast<GLfloat>(shininess_));
    // shader.SetGLfloat("material.opacity", static_cast<GLfloat>(opacity_));

    assert(counters[static_cast<size_t>(Texture::Type::kDiffuse)] > 0);
    assert(counters[static_cast<size_t>(Texture::Type::kNormal)] > 0);
}
