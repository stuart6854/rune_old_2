cmake_minimum_required(VERSION 3.26)
project(Rune)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Distro" CACHE STRING "" FORCE)

include(cmake/CPM.cmake)

add_subdirectory(AssetLib)
add_subdirectory(Rune)
add_subdirectory(Sandbox)