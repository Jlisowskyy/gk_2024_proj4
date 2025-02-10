    message(STATUS "IMGUI fetcher cmake loaded...")

# -------------------------------
# Fetch IMGUI from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libIMGUI
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.8
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(libIMGUI)

file(GLOB IMGUI_SOURCES
        "${libimgui_SOURCE_DIR}/*.cpp"
)

add_library(imgui STATIC
        ${IMGUI_SOURCES}

        # Backend
        ${libimgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${libimgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${libimgui_SOURCE_DIR}
        ${libimgui_SOURCE_DIR}/backends/
)

target_link_libraries(imgui PRIVATE glfw)
