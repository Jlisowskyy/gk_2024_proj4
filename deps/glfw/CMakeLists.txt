message(STATUS "GLFW fetcher cmake loaded...")

# -------------------------------
# Fetch GLFW from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libGLFW
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG master
)

FetchContent_MakeAvailable(libGLFW)
