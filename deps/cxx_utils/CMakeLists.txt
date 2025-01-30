message(STATUS "CXX utils fetcher cmake loaded...")

# -------------------------------
# Fetch cxx_utils from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libCXXUTILS
        GIT_REPOSITORY https://github.com/Jlisowskyy/cxx_utils
        GIT_TAG main
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(libCXXUTILS)
