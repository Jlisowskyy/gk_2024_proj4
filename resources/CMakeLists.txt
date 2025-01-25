set(SOURCE_OUTPUT "static_images.cpp")
set(HEADER_OUTPUT "static_images.hpp")

file(GLOB_RECURSE IMAGE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.png"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.bmp"
)

foreach(IMAGE_FILE ${IMAGE_FILES})
    configure_file(${IMAGE_FILE} ${IMAGE_FILE} COPYONLY)
endforeach()

file(WRITE ${HEADER_OUTPUT}
        "#ifndef STATIC_IMAGES_HPP_\n"
        "#define STATIC_IMAGES_HPP_\n\n"
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticImages {\n"
        "    extern const std::unordered_map<std::string, const unsigned char*> g_KnownImages;\n"
        "    extern const std::unordered_map<std::string, size_t> g_ImageSizes;\n"
        "}\n\n"
        "#endif // STATIC_IMAGES_HPP_\n"
)

file(WRITE ${SOURCE_OUTPUT}
        "#include <unordered_map>\n"
        "#include <string>\n\n"
        "namespace StaticImages {\n"
)

# Process each image
foreach(IMAGE_FILE ${IMAGE_FILES})
    get_filename_component(IMAGE_NAME ${IMAGE_FILE} NAME_WE)

    # Read image file as hex
    file(READ ${IMAGE_FILE} IMAGE_CONTENT HEX)

    # Convert hex to C array
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " IMAGE_HEX_ARRAY ${IMAGE_CONTENT})
    string(REGEX REPLACE ", $" "" IMAGE_HEX_ARRAY ${IMAGE_HEX_ARRAY})

    file(APPEND ${SOURCE_OUTPUT}
            "    const unsigned char g_${IMAGE_NAME}_data[] = { ${IMAGE_HEX_ARRAY} };\n"
            "    const unsigned char* const g_${IMAGE_NAME} = g_${IMAGE_NAME}_data;\n"
            "    const size_t g_${IMAGE_NAME}_size = sizeof(g_${IMAGE_NAME}_data);\n\n"
    )
endforeach()

file(APPEND ${SOURCE_OUTPUT}
        "    std::unordered_map<std::string, const unsigned char*> g_KnownImages = {\n"
)

foreach(IMAGE_FILE ${IMAGE_FILES})
    get_filename_component(IMAGE_NAME ${IMAGE_FILE} NAME_WE)
    file(APPEND ${SOURCE_OUTPUT}
            "        {\"${IMAGE_NAME}\", g_${IMAGE_NAME}},\n"
    )
endforeach()

file(APPEND ${SOURCE_OUTPUT}
        "    };\n"

        "    std::unordered_map<std::string, size_t> g_ImageSizes = {\n"
)

foreach(IMAGE_FILE ${IMAGE_FILES})
    get_filename_component(IMAGE_NAME ${IMAGE_FILE} NAME_WE)
    file(APPEND ${SOURCE_OUTPUT}
            "        {\"${IMAGE_NAME}\", g_${IMAGE_NAME}_size},\n"
    )
endforeach()

file(APPEND ${SOURCE_OUTPUT}
        "    };\n"
        "}\n"
)

# ------------------------------
# Add target
# ------------------------------

file(GLOB_RECURSE LIB_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_library(Resources SHARED
    ${LIB_SOURCES}
)

target_include_directories(Resources PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
