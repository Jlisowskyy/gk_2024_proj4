#include <libcgp/main.hpp>

#include <libcgp/rc.hpp>
#include <libcgp/serialization/scene_serializer.hpp>
#include <libcgp/utils/files.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

static constexpr const char *kDebugSceneDefault = "./scenes/test_scene_1.libgcp_scene";
using namespace LibGcp;

int main()
{
    const std::string dir        = GetDirFromFile(kDebugSceneDefault);
    const std::string scene_name = GetFileName(kDebugSceneDefault);

    SceneSerializer scene_serializer(dir);
    const auto [rc, scene] = scene_serializer.LoadScene(scene_name, SerializationType::kShallow);

    if (IsFailure(rc)) {
        std::cerr << "Failed to load scene: " << kDebugSceneDefault << " caused by: " << GetRcDescription(rc)
                  << std::endl;
        return EXIT_FAILURE;
    }

    return RenderEngineMain(scene);
}
