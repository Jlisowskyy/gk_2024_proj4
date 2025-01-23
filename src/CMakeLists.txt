# ------------------------------
# Find all source files
# ------------------------------

file(GLOB SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# ------------------------------
# Define executable
# ------------------------------

add_executable(render_engine
        ${SOURCES}
)

# ------------------------------
# Add includes
# ------------------------------

target_include_directories(render_engine PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
