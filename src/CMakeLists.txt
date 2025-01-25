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
