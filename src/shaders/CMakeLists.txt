set(SOURCE_OUTPUT "static_shaders.cpp")
set(HEADER_OUTPUT "static_header.hpp")

file(GLOB_RECURSE SHADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl"
)

# Generate header file
file(WRITE ${HEADER_OUTPUT}
        "#ifndef SHADERS_STATIC_SHADERS_HPP_\n"
        "#define SHADERS_STATIC_SHADERS_HPP_\n\n"
        "namespace StaticShaders {\n"
)

# Generate cpp file
file(WRITE ${SOURCE_OUTPUT}
        "#include \"${HEADER_OUTPUT}\"\n\n"
        "namespace StaticShaders {\n"
)

foreach(SHADER_FILE ${SHADER_FILES})
    message(STATUS "Detected shader: ${SHADER_FILE}")

    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    file(READ ${SHADER_FILE} SHADER_CONTENTS)

    # Escape backslashes and quotes in shader contents
    string(REPLACE "\\" "\\\\" SHADER_CONTENTS "${SHADER_CONTENTS}")
    string(REPLACE "\"" "\\\"" SHADER_CONTENTS "${SHADER_CONTENTS}")

    # Write extern declaration in header
    file(APPEND ${HEADER_OUTPUT}
            "    extern const char* const ${SHADER_NAME};\n"
    )

    # Write variable definition in cpp
    file(APPEND ${SOURCE_OUTPUT}
            "    const char* const ${SHADER_NAME} = \nR\"(${SHADER_CONTENTS})\";\n\n"
    )
endforeach()

file(APPEND ${HEADER_OUTPUT}
        "}\n\n"
        "#endif // SHADERS_STATIC_SHADERS_HPP_\n"
)

file(APPEND ${SOURCE_OUTPUT}
        "}\n"
)
