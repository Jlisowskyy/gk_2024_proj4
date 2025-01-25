message(STATUS "GLAD fetcher cmake loaded...")

# -------------------------------
# Fetch GLAD from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG v2.0.8
        GIT_PROGRESS   TRUE
        SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)

glad_add_library(glad STATIC LANG C++ API gl:core=4.6)
