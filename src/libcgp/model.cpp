#include <libcgp/mesh.hpp>
#include <libcgp/model.hpp>
#include <libcgp/utils/macros.hpp>

#include <cassert>
#include <cstddef>
#include <string>
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

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::LoadModelFromExternalFormat(const std::string &path)
{
    meshes_.clear();
    Assimp::Importer importer{};

    const auto *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
                  aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes
    );

    VALIDATE_STATE(
        scene != nullptr && scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE && scene->mRootNode != nullptr,
        importer.GetErrorString()
    );

    directory_ = path.substr(0, path.find_last_of('/'));

    ProcessNode_(scene->mRootNode, scene);
    return std::make_shared<Model>(std::move(meshes_));
}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::LoadModelFromInternalFormat(const std::string &){NOT_IMPLEMENTED
}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::DumpModelToInternalFormat(const Model &, const std::string &)
{
    NOT_IMPLEMENTED
}

void LibGcp::ModelSerializer::ProcessNode_(const aiNode *node, const aiScene *scene)
{
    assert(node != nullptr);
    assert(scene != nullptr);

    for (size_t idx = 0; idx < node->mNumMeshes; ++idx) {
        const auto *mesh = scene->mMeshes[node->mMeshes[idx]];
        meshes_.push_back(ProcessMesh_(mesh, scene));
    }

    for (size_t idx = 0; idx < node->mNumChildren; ++idx) {
        ProcessNode_(node->mChildren[idx], scene);
    }
}

std::shared_ptr<LibGcp::Mesh> LibGcp::ModelSerializer::ProcessMesh_(const aiMesh *mesh, const aiScene *scene)
{
    assert(mesh != nullptr);
    assert(scene != nullptr);

    std::vector<Mesh::Vertex> vertices{};
    std::vector<GLuint> indices{};
    std::vector<Texture> textures{};

    // process vertices
    vertices.reserve(mesh->mNumVertices);
    for (size_t idx = 0; idx < mesh->mNumVertices; ++idx) {
        vertices.emplace_back(
            glm::vec3{mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z},
            glm::vec3{mesh->mNormals[idx].x, mesh->mNormals[idx].y, mesh->mNormals[idx].z},
            mesh->mTextureCoords[0] ? glm::vec2{mesh->mTextureCoords[0][idx].x, mesh->mTextureCoords[0][idx].y}
                                    : glm::vec2{0.0F, 0.0F}
        );
    }

    // precalculate the number of indices
    size_t num_indices = 0;
    for (size_t idx = 0; idx < mesh->mNumFaces; ++idx) {
        num_indices += mesh->mFaces[idx].mNumIndices;
    }

    // process indices
    indices.reserve(num_indices);
    for (size_t idx = 0; idx < mesh->mNumFaces; ++idx) {
        const auto &face = mesh->mFaces[idx];
        for (size_t jdx = 0; jdx < face.mNumIndices; ++jdx) {
            indices.push_back(face.mIndices[jdx]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // precalculate the number of textures
    const size_t num_textures =
        material->GetTextureCount(aiTextureType_DIFFUSE) + material->GetTextureCount(aiTextureType_SPECULAR);

    // process textures
    textures.reserve(num_textures);
    LoadMaterialTextures_(textures, material, aiTextureType_DIFFUSE, Texture::Type::kDiffuse);
    LoadMaterialTextures_(textures, material, aiTextureType_SPECULAR, Texture::Type::kSpecular);

    return std::make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(textures));
}

void LibGcp::ModelSerializer::LoadMaterialTextures_(
    std::vector<Texture> &textures, const aiMaterial *material, const aiTextureType type,
    const Texture::Type texture_type
)
{
    for (size_t idx = 0; idx < material->GetTextureCount(type); ++idx) {
        aiString str;
        material->GetTexture(type, idx, &str);

        const auto full_path = textures.emplace_back(std::move(MakeTextureFromFile(, texture_type)));
    }
}
