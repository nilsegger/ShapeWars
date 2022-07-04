# CMakeList.txt : CMake project for Shapes, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_STANDARD 11) # Requires C11 standard

add_subdirectory(raylib)

# Add source to this project's executable.
add_executable (Shapes "Shapes.c")

target_include_directories(Shapes PRIVATE raylib)
target_link_libraries(Shapes raylib)

# TODO: Add tests and install targets if needed.
