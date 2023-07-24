cmake_minimum_required(VERSION 3.15)

project(Rune VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# Dependencies
include("Dependencies.cmake")
add_subdirectory(vendor)

# Libraries
add_subdirectory(Rune)

#Executables
add_subdirectory(Sandbox)