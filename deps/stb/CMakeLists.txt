message(STATUS "STB fetcher cmake loaded...")

include(FetchContent)

FetchContent_Declare(
        libSTB
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(libSTB)

add_library(libStb INTERFACE)

target_include_directories(libStb INTERFACE
        ${libstb_SOURCE_DIR}
)

