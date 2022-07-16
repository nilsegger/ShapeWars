# CMakeList.txt : CMake project for Shapes, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_CXX_STANDARD 14)

set(SHAPES_SOURCES
	"include/cells.h"
	"src/cells.c"
	"include/structs.h"
	"include/helpers.h"
	"include/collision.h"
 "src/structs.c")

add_subdirectory(raylib)

include_directories("include/")

# Add source to this project's executable.
add_executable (Shapes)

target_sources(Shapes
	PUBLIC 
	"src/main.c"
	PRIVATE
	${SHAPES_SOURCES}
)

target_include_directories(Shapes PRIVATE raylib)
target_link_libraries(Shapes raylib_static)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shapes_test)
target_sources(shapes_test PUBLIC 
	"tests/collision_test.cpp"
	PRIVATE
	${SHAPES_SOURCES}
)

target_link_libraries(
	shapes_test
	raylib
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(shapes_test)
