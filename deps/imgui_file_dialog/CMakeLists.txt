message(STATUS "IMGUI FILE DIALOG fetcher cmake loaded...")

# -------------------------------
# Fetch IMGUI FILE DIALOG from github...
# -------------------------------

include(FetchContent)

FetchContent_Declare(
        libIMGUIFD
        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog
        GIT_TAG v0.6.7
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(libIMGUIFD)

target_link_libraries(ImGuiFileDialog PRIVATE imgui)
