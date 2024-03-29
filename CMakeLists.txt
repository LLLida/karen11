cmake_minimum_required(VERSION 3.8)
project(karen VERSION 1.1 LANGUAGES C CXX)
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

set(default_build_type "Release")

# Use C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_KAREN "Build karen with command line interface" ON)

# Show all warnings and treat them as errors
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Werror -Wall -Wextra -Wpedantic")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/W4 /Wall /WX ")
endif()

add_executable(karen "src/karen.cpp" "src/ConsolePlay.cpp")

# Enable parallel computation
target_compile_definitions(${PROJECT_NAME} PRIVATE "KAREN_ENABLE_PARALLEL")

# Enable Link Time Optimization when release
if (CMAKE_BUILD_TYPE MATCHES RELEASE)
  set_property(TARGET karen PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Link statically
  set(CMAKE_EXE_LINKER_FLAGS " -static")   
  target_link_libraries(karen "-static-libgcc" "-static-libstdc++")
endif()

