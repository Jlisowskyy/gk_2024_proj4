#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/mesh.hpp>
#include <libcgp/primitives/model.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/utils/timer.hpp>

#include <cassert>
#include <cstddef>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

// ------------------------------
// Static helpers
// ------------------------------

L_FAST_CALL void TraceSceneInfo(const aiScene *scene)
{
    if (scene->HasLights()) {
        TRACE("[SCENE INFO] Scene has lights!");
    }

    if (scene->HasAnimations()) {
        TRACE("[SCENE INFO] Scene has animations!");
    }

    if (scene->HasCameras()) {
        TRACE("[SCENE INFO] Scene has cameras!");
    }

    if (scene->hasSkeletons()) {
        TRACE("[SCENE INFO] Scene has skeletons!");
    }

    if (scene->HasMaterials()) {
        TRACE("[SCENE INFO] Scene has materials!");
    }

    for (size_t idx = 0; idx < scene->mMetaData->mNumProperties; ++idx) {
        TRACE("[SCENE INFO] " << scene->mMetaData->mKeys[idx].C_Str());
    }
}

L_FAST_CALL void TraceMeshInfo(const aiMesh *mesh)
{
    if (mesh->HasBones()) {
        TRACE("[MESH INFO] Mesh has bones!");
    }

    if (mesh->HasFaces()) {
        TRACE("[MESH INFO] Mesh has faces!");
    }

    if (mesh->HasNormals()) {
        TRACE("[MESH INFO] Mesh has normals!");
    }

    if (mesh->HasTangentsAndBitangents()) {
        TRACE("[MESH INFO] Mesh has tangents and bitangents!");
    }
}

L_FAST_CALL void TraceMaterialInfo(aiMaterial *material)
{
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
        TRACE("[MATERIAL INFO] Material has normal maps!");
    }

    if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0) {
        TRACE("[MATERIAL INFO] Material has ambient occlusion maps!");
    }

    for (size_t idx = 0; idx < material->mNumProperties; ++idx) {
        TRACE("[MATERIAL INFO] " << material->mProperties[idx]->mKey.C_Str());
    }
}

// ------------------------------
// Implementations
// ------------------------------

LibGcp::Model::Model(std::vector<std::shared_ptr<Mesh> > &&meshes) : meshes_(std::move(meshes)) {}

void LibGcp::Model::Draw(Shader &shader) const
{
    for (const auto &mesh : meshes_) {
        mesh->Draw(shader);
    }
}

std::shared_ptr<LibGcp::Model> LibGcp::ModelSerializer::LoadModelFromExternalFormat(const std::string &path)
{
    Timer timer{};
    timer.Start(0);

    meshes_.clear();
    Assimp::Importer importer{};

    const auto *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
                  aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes
    );

    if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
        TRACE(importer.GetErrorString());
        return nullptr;
    }

    full_path_ = path;
    directory_ = path.substr(0, path.find_last_of('/'));

    TraceSceneInfo(scene);
    ProcessNode_(scene->mRootNode, scene);
    timer.StopAndPrint(0);

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

    std::vector<Vertex> vertices{};
    std::vector<GLuint> indices{};
    std::vector<std::shared_ptr<Texture> > textures{};

    TraceMeshInfo(mesh);

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
    TraceMaterialInfo(material);

    // precalculate the number of textures
    const size_t num_textures =
        material->GetTextureCount(aiTextureType_DIFFUSE) + material->GetTextureCount(aiTextureType_SPECULAR);

    // process textures
    textures.reserve(num_textures);
    LoadMaterialTextures_(textures, scene, material, aiTextureType_DIFFUSE, Texture::Type::kDiffuse);
    LoadMaterialTextures_(textures, scene, material, aiTextureType_SPECULAR, Texture::Type::kSpecular);
    LoadMaterialTextures_(textures, scene, material, aiTextureType_NORMALS, Texture::Type::kNormal);

    auto mesh_ptr = std::make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(textures));

    /* process properties */
    float shininess;
    R_ASSERT(material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS);

    float opacity;
    R_ASSERT(material->Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS);

    mesh_ptr->GetShininess() = shininess;
    mesh_ptr->GetOpacity()   = opacity;

    return mesh_ptr;
}

void LibGcp::ModelSerializer::LoadMaterialTextures_(
    std::vector<std::shared_ptr<Texture> > &textures, const aiScene *scene, const aiMaterial *material,
    const aiTextureType type, const Texture::Type texture_type
)
{
    const std::filesystem::path dir_path = std::filesystem::absolute(directory_);

    for (size_t idx = 0; idx < material->GetTextureCount(type); ++idx) {
        aiString str;
        material->GetTexture(type, idx, &str);

        /* check texture type */
        const aiTexture *ai_texture = scene->GetEmbeddedTexture(str.C_Str());

        std::shared_ptr<Texture> texture;
        if (ai_texture == nullptr) {
            const std::filesystem::path texture_full_path = weakly_canonical(dir_path / str.C_Str());

            texture = ResourceMgr::GetInstance().GetTexture({
                .paths           = {texture_full_path.string()},
                .type            = ResourceType::kTexture,
                .load_type       = LoadType::kExternal,
                .is_serializable = false,
            });
        } else {
            const std::string full_path = full_path_ + "/" + str.C_Str();

            texture = ResourceMgr::GetInstance().GetTextureExternalSourceRaw(
                full_path,
                {
                    .texture_data = reinterpret_cast<unsigned char *>(ai_texture->pcData),
                    .width        = static_cast<int>(ai_texture->mWidth),
                    .height       = static_cast<int>(ai_texture->mHeight),
                    .channels     = 4,
                }
            );
        }

        texture->SetType(texture_type);
        textures.push_back(texture);
    }
}
