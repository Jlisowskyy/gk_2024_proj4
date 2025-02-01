#ifndef LIBGCP_MODEL_HPP_
#define LIBGCP_MODEL_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/primitives/mesh.hpp>
#include <libcgp/primitives/texture.hpp>

#include <memory>
#include <string>
#include <vector>

#include <assimp/material.h>

/* Forward declarations */
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

LIBGCP_DECL_START_

/* Forward declarations */
class Shader;

// ------------------------------
// Model class
// ------------------------------

class Model
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    Model() = delete;

    ~Model() = default;

    Model(Model &&) noexcept = default;

    Model &operator=(Model &&) noexcept = default;

    Model(const Model &) = delete;

    Model &operator=(const Model &) = delete;

    explicit Model(std::vector<std::shared_ptr<Mesh>> &&meshes);

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Draw(Shader &shader) const;

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<std::shared_ptr<Mesh>> meshes_;
};

// ------------------------------
// Model serializer
// ------------------------------

class ModelSerializer
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    ModelSerializer() = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    NDSCRD std::shared_ptr<Model> LoadModelFromExternalFormat(const std::string &path);

    NDSCRD std::shared_ptr<Model> LoadModelFromInternalFormat(const std::string &path);

    NDSCRD std::shared_ptr<Model> DumpModelToInternalFormat(const Model &model, const std::string &path);

    // ----------------------------------
    // Class implementation methods
    // ----------------------------------

    protected:
    void ProcessNode_(const aiNode *node, const aiScene *scene);
    std::shared_ptr<Mesh> ProcessMesh_(const aiMesh *mesh, const aiScene *scene);
    void LoadMaterialTextures_(
        std::vector<std::shared_ptr<Texture>> &textures, const aiScene *scene, const aiMaterial *material,
        aiTextureType type, Texture::Type texture_type
    );

    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<std::shared_ptr<Mesh>> meshes_{};
    std::string directory_{};
    std::string full_path_{};
};

LIBGCP_DECL_END_

#endif  // LIBGCP_MODEL_HPP_
