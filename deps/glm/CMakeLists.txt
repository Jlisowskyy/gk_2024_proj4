message(STATUS "GLM fetcher cmake loaded...")

# -------------------------------
# Fetch GLM from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libGLM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
        GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(libGLM)
