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

# Ensure main.cpp removed from lib sources
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

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
)

# ------------------------------
# Define executable
# ------------------------------

add_executable(${EXEC_NAME}
        main.cpp
)

# ------------------------------
# Link lib to executable
# ------------------------------

target_link_libraries(${EXEC_NAME} ${LIB_NAME})

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
