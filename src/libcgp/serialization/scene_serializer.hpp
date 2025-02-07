#ifndef SERIALIZATION_SCENE_SERIALIZER_HPP_
#define SERIALIZATION_SCENE_SERIALIZER_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/rc.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

LIBGCP_DECL_START_
class SceneSerializer
{
    public:
    // ------------------------------
    // Inner types
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    explicit SceneSerializer(const std::string &output_dir) : output_dir_(output_dir) {}

    // ------------------------------
    // Class interaction
    // ------------------------------

    /* Without file format */
    Rc SerializeScene(const std::string &scene_name, SerializationType type);

    Scene LoadScene(const std::string &scene_name, SerializationType type);

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    Rc _serializeSceneShallow(const std::string &scene_name);

    Rc _serializeSceneDeep(const std::string &scene_name);

    Rc _serializeSceneDeepPack(const std::string &scene_name);

    Rc _serializeSceneDeepAttach(const std::string &scene_name);

    std::vector<SceneSerialized::SettingsSerialized> _serializeSettings();

    std::vector<SceneSerialized::ResourceSerialized> _serializeResources();

    std::vector<SceneSerialized::StaticObjectSerialized> _serializeStaticObjects();

    size_t GetStringId_(const std::string &name);

    // ------------------------------
    // Class fields
    // ------------------------------

    size_t string_counter_{};
    std::unordered_map<std::string, size_t> string_map_;

    std::string output_dir_;
};

LIBGCP_DECL_END_

#endif  // SERIALIZATION_SCENE_SERIALIZER_HPP_
