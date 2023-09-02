cmake_minimum_required(VERSION 3.26)

project(
    Tetris
    VERSION 0.1.0
    LANGUAGES CXX
    )
set(CMAKE_CXX_STANDARD 20)
# In order to support C++20 modules, 
# there are a number of behaviors that have CMake APIs to provide the required features to build and export them from a project.
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  include(gcc_modules.cmake)
endif()

include(FetchContent)

FetchContent_Declare(SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG 031912c4b6c5db80b443f04aa56fec3e4e645153
  FIND_PACKAGE_ARGS NAMES SDL2
)

# This will try calling find_package() first for both dependencies
FetchContent_MakeAvailable(SDL2)

include(FindVulkan)

file(GLOB SRC src/*.cpp)
file(GLOB MODULE src/*.cxx)

add_executable(tetris ${SRC})
target_sources(tetris
    PRIVATE
        FILE_SET modules 
        TYPE CXX_MODULES 
        FILES ${MODULE}
        )

# SDL2::SDL2main may or may not be available.
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(tetris PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(tetris PRIVATE SDL2::SDL2)