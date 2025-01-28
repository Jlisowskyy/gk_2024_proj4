message(STATUS "ASSIMP fetcher cmake loaded...")

# -------------------------------
# Fetch ASSIMP from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libASSIMP
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(libASSIMP)
