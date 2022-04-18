cmake_minimum_required(VERSION 3.0)
project(sdrtools)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# TODO: Add common library for all tools.
# add_subdirectory(lib)

add_subdirectory(bin)

# Enable address sanitizers for all
# if in debug mode.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(-fsanitize=address,undefined)
  target_link_options(-fsanitize=address,undefined)
endif()
  
