set(SOURCE_OUTPUT "static_shaders.cpp")
set(HEADER_OUTPUT "static_header.hpp")

file(GLOB_RECURSE FRAG_SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)

file(GLOB_RECURSE VERT_SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
)

function(ConfigureFiles files)
    foreach(SHADER_FILE ${files})
        configure_file(${SHADER_FILE} ${SHADER_FILE} COPYONLY)
    endforeach()
endfunction()

ConfigureFiles(${FRAG_SHADER_FILES})
ConfigureFiles(${VERT_SHADER_FILES})

file(WRITE ${HEADER_OUTPUT}
        "#ifndef SHADERS_STATIC_SHADERS_HPP_\n"
        "#define SHADERS_STATIC_SHADERS_HPP_\n\n"
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticShaders {\n"
        "    extern std::unordered_map<std::string, const char*> g_KnownVertexShaders;\n"
        "    extern std::unordered_map<std::string, const char*> g_KnownFragmentShaders;\n"
        "}\n\n"
        "#endif // SHADERS_STATIC_SHADERS_HPP_\n"
)

file(WRITE ${SOURCE_OUTPUT}
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticShaders {\n"
)

function(AddShaders prefix files)
    foreach(SHADER_FILE ${files})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        file(READ ${SHADER_FILE} SHADER_CONTENTS)

        string(REPLACE "\\" "\\\\" SHADER_CONTENTS "${SHADER_CONTENTS}")
        string(REPLACE "\"" "\\\"" SHADER_CONTENTS "${SHADER_CONTENTS}")

        file(APPEND ${SOURCE_OUTPUT}
                "    const char* const g_${prefix}_${SHADER_NAME} = \nR\"(${SHADER_CONTENTS})\";\n\n"
        )
    endforeach()
endfunction()

AddShaders("frag" ${FRAG_SHADER_FILES})
AddShaders("vert" ${VERT_SHADER_FILES})

function(AddMap map_name files prefix)
    file(APPEND ${SOURCE_OUTPUT}
            "    std::unordered_map<std::string, const char*> ${map_name} = {\n"
    )

    foreach(SHADER_FILE ${files})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        file(APPEND ${SOURCE_OUTPUT}
                "        {\"${SHADER_NAME}\", g_${prefix}_${SHADER_NAME}},\n"
        )
    endforeach()

    file(APPEND ${SOURCE_OUTPUT}
            "    };\n"
    )
endfunction()

AddMap(g_KnownFragmentShaders ${FRAG_SHADER_FILES} frag)
AddMap(g_KnownVertexShaders ${VERT_SHADER_FILES} vert)

file(APPEND ${SOURCE_OUTPUT}
        "}\n"
)