message(STATUS "ASSIMP fetcher cmake loaded...")

# -------------------------------
# Fetch ASSIMP from github...
# -------------------------------

include(FetchContent)

set(ASSIMP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp as a static library")
set(ASSIMP_INSTALL OFF CACHE BOOL "Do not install Assimp")
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "Build own zlib")
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Warning no problems")

FetchContent_Declare(
        libASSIMP
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG v5.4.3
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(libASSIMP)
