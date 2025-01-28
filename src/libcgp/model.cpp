#include <libcgp/mesh.hpp>
#include <libcgp/model.hpp>
#include <libcgp/utils/macros.hpp>

#include <utility>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

LibGcp::Model::Model(std::vector<std::shared_ptr<Mesh> > &&meshes) : meshes_(std::move(meshes)) {}

void LibGcp::Model::Draw(Shader &shader) const
{
    for (const auto &mesh : meshes_) {
        mesh->Draw(shader);
    }
}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::LoadModelFromExternalFormat(const std::string &path) {}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::LoadModelFromInternalFormat(const std::string &){NOT_IMPLEMENTED
}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::DumpModelToInternalFormat(const Model &, const std::string &)
{
    NOT_IMPLEMENTED
}
