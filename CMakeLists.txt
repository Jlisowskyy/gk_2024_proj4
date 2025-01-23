cmake_minimum_required(VERSION 3.29)
project(OpenGLTestScene C CXX)

set(PROJECT_NAME OpenGLTestScene)
set(PROJECT_VERSION "1.0")
set(PROJECT_AUTHOR "Jakub Lisowski")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# Load all deps
add_subdirectory(deps)

# Load project
add_subdirectory(src)
