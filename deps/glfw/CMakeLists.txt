message(STATUS "GLFW fetcher cmake loaded...")

# -------------------------------
# Fetch GLFW from github...
# -------------------------------

include(FetchContent)

set(GLFW_BUILD_WAYLAND 0)
FetchContent_Declare(
        libGLFW
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(libGLFW)
