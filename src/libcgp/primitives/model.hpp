#ifndef LIBGCP_MODEL_HPP_
#define LIBGCP_MODEL_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/engine/lights.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/primitives/mesh.hpp>
#include <libcgp/primitives/texture.hpp>

#include <CxxUtils/data_types/multi_vector.hpp>

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

using LightContainer = CxxUtils::MultiVector<PointLight, SpotLight>;

// ------------------------------
// Model class
// ------------------------------

class Model : public Resource
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

    FAST_CALL void Draw(Shader &shader, const RenderPass pass) const
    {
        for (const auto &mesh : meshes_) {
            mesh->Draw(shader, pass);
        }
    }

    NDSCRD FAST_CALL size_t GetMeshesCount() const { return meshes_.size(); }

    NDSCRD FAST_CALL std::shared_ptr<Mesh> GetMesh(const size_t idx) const { return meshes_[idx]; }

    NDSCRD FAST_CALL LightContainer &GetLights() { return lights_; }

    NDSCRD FAST_CALL const LightContainer &GetLights() const { return lights_; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    LightContainer lights_{};
    std::vector<std::shared_ptr<Mesh>> meshes_{};
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

    void FallBackToColor(std::vector<std::shared_ptr<Texture>> &textures, const aiMaterial *material);

    void FallBackNormal(std::vector<std::shared_ptr<Texture>> &textures);

    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<std::shared_ptr<Mesh>> meshes_{};
    std::string directory_{};
    std::string full_path_{};
    std::string format_{"unknown"};
};

LIBGCP_DECL_END_

#endif  // LIBGCP_MODEL_HPP_
