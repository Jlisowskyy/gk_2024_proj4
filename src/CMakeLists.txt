# ------------------------------
# Preprocess shaders
# ------------------------------

add_subdirectory(shaders)

# ------------------------------
# Find all source files
# ------------------------------

file(GLOB_RECURSE LIB_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

message(STATUS "Files: ${LIB_SOURCES}")

# Ensure mains removed from lib sources
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main_debug.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main_editor.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main_game.cpp")

# ------------------------------
# Define static library
# ------------------------------

add_library(${LIB_NAME} STATIC
        ${LIB_SOURCES}
)

# ------------------------------
# Add includes
# ------------------------------

target_include_directories(${LIB_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ------------------------------
# Lib links
# ------------------------------

find_package(OpenGL)

target_link_libraries(${LIB_NAME} PUBLIC
        glfw
        OpenGL
        glad
        libStb
        glm-header-only
        assimp::assimp
        CxxUtilsLib
        imgui
        ImGuiFileDialog
)

# ------------------------------
# Define executable
# ------------------------------

add_executable(${EXEC_NAME}_debug
        main_debug.cpp
)

add_executable(${EXEC_NAME}_game
        main_game.cpp
)

add_executable(${EXEC_NAME}_editor
        main_editor.cpp
)

# ------------------------------
# Link lib to executable
# ------------------------------

target_link_libraries(${EXEC_NAME}_debug PRIVATE ${LIB_NAME})
target_link_libraries(${EXEC_NAME}_game PRIVATE ${LIB_NAME})
target_link_libraries(${EXEC_NAME}_editor PRIVATE ${LIB_NAME})

# ------------------------------
# Add compile options
# ------------------------------

if (DEFINED USE_TRACE AND USE_TRACE)
    message(STATUS "Enabling traces...")
    target_compile_definitions(${LIB_NAME} PUBLIC USE_TRACES_=1)
endif ()

if (DEFINED USE_TIMERS AND USE_TIMERS)
    message(STATUS "Enabling timers...")
    target_compile_definitions(${LIB_NAME} PUBLIC USE_TIMERS_=1)
endif ()

#target_compile_definitions(${LIB_NAME} PUBLIC UNIFORMS_DROPS_WHEN_NOT_FOUND_=1)

# copy exec to repo dir after build

add_custom_command(TARGET ${EXEC_NAME}_editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${EXEC_NAME}_editor> ${REPO_DIR}/${EXEC_NAME}_editor
        COMMENT "Copying ${EXEC_NAME}_editor to ${REPO_DIR}/${EXEC_NAME}_editor"
)
