cmake_minimum_required(VERSION 3.8)

project(karen VERSION 1.1 LANGUAGES CXX)

# Use C++17 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Show all warnings and treat them as errors
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Werror -Wall -Wextra -Wpedantic")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/W4 /Wall /WX ")
endif()

add_executable(${PROJECT_NAME} "src/main.cpp" "src/ConsolePlay.cpp")

# Enable parallel computation
target_compile_definitions(${PROJECT_NAME} PRIVATE "KAREN_ENABLE_PARALLEL")

# Enable Link Time Optimization when release
if (CMAKE_BUILD_TYPE MATCHES RELEASE)
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Link statically
  set(CMAKE_EXE_LINKER_FLAGS " -static")
  target_link_libraries(${CMAKE_PROJECT_NAME} -static-libgcc -static-libstdc++) 
  
endif()

