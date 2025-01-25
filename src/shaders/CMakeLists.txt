set(SOURCE_OUTPUT "static_shaders.cpp")
set(HEADER_OUTPUT "static_header.hpp")

file(GLOB_RECURSE SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl"
)

foreach(SHADER_FILE ${SHADER_FILES})
    configure_file(${SHADER_FILE} ${SHADER_FILE} COPYONLY)
endforeach()

file(WRITE ${HEADER_OUTPUT}
        "#ifndef SHADERS_STATIC_SHADERS_HPP_\n"
        "#define SHADERS_STATIC_SHADERS_HPP_\n\n"
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticShaders {\n"
        "    extern std::unordered_map<std::string, const char*> g_KnownShaders;\n"
)

file(WRITE ${SOURCE_OUTPUT}
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticShaders {\n"
)

foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    file(READ ${SHADER_FILE} SHADER_CONTENTS)

    string(REPLACE "\\" "\\\\" SHADER_CONTENTS "${SHADER_CONTENTS}")
    string(REPLACE "\"" "\\\"" SHADER_CONTENTS "${SHADER_CONTENTS}")

    file(APPEND ${HEADER_OUTPUT}
            "    extern const char* const g_${SHADER_NAME};\n"
    )

    file(APPEND ${SOURCE_OUTPUT}
            "    const char* const g_${SHADER_NAME} = \nR\"(${SHADER_CONTENTS})\";\n\n"
    )
endforeach()

file(APPEND ${SOURCE_OUTPUT}
        "    std::unordered_map<std::string, const char*> g_KnownShaders = {\n"
)

foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    file(APPEND ${SOURCE_OUTPUT}
            "        {\"${SHADER_NAME}\", g_${SHADER_NAME}},\n"
    )
endforeach()

file(APPEND ${SOURCE_OUTPUT}
        "    };\n"
)

file(APPEND ${HEADER_OUTPUT}
        "}\n\n"
        "#endif // SHADERS_STATIC_SHADERS_HPP_\n"
)

file(APPEND ${SOURCE_OUTPUT}
        "}\n"
)