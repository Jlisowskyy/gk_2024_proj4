#ifndef MGR_RESOURCE_MGR_HPP_
#define MGR_RESOURCE_MGR_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/primitives/model.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/texture.hpp>
#include <libcgp/rc.hpp>

#include <CxxUtils/singleton.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

LIBGCP_DECL_START_
/**
 *   Possible resource types:
 *   - textures
 *   - shaders
 *   - models
 */

/**
 * TODO:
 * - flyweight pattern for meshes
 * - memory usage optimization
 * - implement multi-threaded async loading
 */
class ResourceMgr final : public CxxUtils::Singleton<ResourceMgr>
{
    static constexpr size_t kDefaultMapSize = 16384;

    // ------------------------------
    // Inner types
    // ------------------------------

    public:
    enum class ResourceType : std::uint8_t {
        kTexture,
        kShader,
        kModel,
        kLast,
    };

    enum class LoadType : std::uint8_t {
        kExternal,
        kMemory,
        kInternal,
        kLast,
    };

    struct alignas(128) ResourceSpec {
        std::array<std::string, 2> paths{};
        ResourceType type{};
        LoadType load_type{};
        int8_t flip_texture{-1};
    };

    using resource_t = std::vector<ResourceSpec>;

    // ------------------------------
    // Object creation
    // ------------------------------

    protected:
    ResourceMgr() = default;

    public:
    ~ResourceMgr();

    FAST_CALL static ResourceMgr &InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new ResourceMgr());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    ResourceMgr &Init(const resource_t &resources);

    std::shared_ptr<Texture> GetTexture(const ResourceSpec &resource);
    std::shared_ptr<Shader> GetShader(const ResourceSpec &resource);
    std::shared_ptr<Model> GetModel(const ResourceSpec &resource);

    std::shared_ptr<Texture> GetTexture(const std::string &texture_name, LoadType load_type);
    std::shared_ptr<Shader> GetShader(const std::string &shader_name, LoadType load_type);
    std::shared_ptr<Model> GetModel(const std::string &model_name, LoadType load_type);

    std::shared_ptr<Texture> GetTextureExternalSourceRaw(const std::string &path, const TextureSpec &spec);

    FAST_CALL const std::unordered_map<std::string, std::shared_ptr<Texture>> &GetTextures() const { return textures_; }

    FAST_CALL const std::unordered_map<std::string, std::shared_ptr<Shader>> &GetShaders() const { return shaders_; }

    FAST_CALL const std::unordered_map<std::string, std::shared_ptr<Model>> &GetModels() const { return models_; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    Rc LoadTextureUnlocked_(const ResourceSpec &resource);

    Rc LoadShaderUnlocked_(const ResourceSpec &resource);

    Rc LoadModelUnlocked_(const ResourceSpec &resource);

    Rc LoadTextureFromExternal_(const ResourceSpec &resource);

    std::shared_ptr<Texture> LoadTextureFromMemory_(const unsigned char *data, int len);

    Rc LoadShaderFromMemory_(const ResourceSpec &resource);

    Rc LoadShaderFromExternal_(const ResourceSpec &resource);

    Rc LoadModelFromExternal_(const ResourceSpec &resource);

    // ------------------------------
    // Class fields
    // ------------------------------

    std::mutex texture_mutex_{};
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_{};

    std::mutex shader_mutex_{};
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_{};

    std::mutex model_mutex_{};
    std::unordered_map<std::string, std::shared_ptr<Model>> models_{};
};

LIBGCP_DECL_END_

#endif  // MGR_RESOURCE_MGR_HPP_
