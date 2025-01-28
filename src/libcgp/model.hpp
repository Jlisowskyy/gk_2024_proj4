#ifndef LIBGCP_MODEL_HPP_
#define LIBGCP_MODEL_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/mesh.hpp>

#include <memory>
#include <string>
#include <vector>

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

    explicit Model(std::vector<std::shared_ptr<Mesh> > &&meshes);

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

    std::vector<std::shared_ptr<Mesh> > meshes_;
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
    // ------------------------------
    // Class fields
    // ------------------------------

    std::string directory_;
};

LIBGCP_DECL_END_

#endif  // LIBGCP_MODEL_HPP_
