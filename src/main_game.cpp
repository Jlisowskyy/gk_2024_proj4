#include <libcgp/main.hpp>
#include <libcgp/serialization/scene_serializer.hpp>
#include <libcgp/utils/files.hpp>

#include <cstdlib>
#include <iostream>

using namespace LibGcp;

int main(const int argc, const char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path to scene>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string scene_path = argv[1];
    const std::string dir        = GetDirFromFile(scene_path);
    const std::string scene_name = GetFileName(scene_path);

    SceneSerializer scene_serializer(dir);
    const auto [rc, scene] = scene_serializer.LoadScene(scene_name, SerializationType::kShallow);

    if (IsFailure(rc)) {
        std::cerr << "Failed to load scene: " << scene_path << " caused by: " << GetRcDescription(rc) << std::endl;
        return EXIT_FAILURE;
    }

    return RenderEngineMain(scene);
}
