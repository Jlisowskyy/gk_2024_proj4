#ifndef SERIALIZATION_SCENE_SERIALIZER_HPP_
#define SERIALIZATION_SCENE_SERIALIZER_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/rc.hpp>

#include <string>
#include <tuple>
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

    std::tuple<Rc, Scene> LoadScene(const std::string &scene_name, SerializationType type);

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    Rc SerializeSceneShallow_(const std::string &scene_name);

    Rc SerializeSceneDeep_(const std::string &scene_name);

    Rc SerializeSceneDeepPack_(const std::string &scene_name);

    Rc SerializeSceneDeepAttach_(const std::string &scene_name);

    std::vector<SceneSerialized::SettingsSerialized> SerializeSettings_();

    std::vector<SceneSerialized::ResourceSerialized> SerializeResources_();

    std::vector<SceneSerialized::StaticObjectSerialized> SerializeStaticObjects_();

    CxxUtils::MultiVector<SceneSerialized::PointLightSerialized, SceneSerialized::SpotLightSerialized> SerializeLights_(
    );

    void SaveStringTable(std::ofstream &file);

    size_t GetStringId_(const std::string &name);

    std::tuple<Rc, Scene> LoadSceneShallow_(const std::string &scene_name) const;

    std::tuple<Rc, Scene> LoadSceneDeep_(const std::string &scene_name);

    std::tuple<Rc, Scene> LoadSceneDeepPack_(const std::string &scene_name);

    std::tuple<Rc, Scene> LoadSceneDeepAttach_(const std::string &scene_name);

    Rc VerifyFile_(const std::string &file_name) const;

    // ------------------------------
    // Class fields
    // ------------------------------

    size_t string_counter_{};
    std::unordered_map<std::string, size_t> string_map_;

    std::string output_dir_;
};

LIBGCP_DECL_END_

#endif  // SERIALIZATION_SCENE_SERIALIZER_HPP_
