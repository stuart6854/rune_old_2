add_subdirectory(submodules)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_library(mono SHARED IMPORTED)
target_include_directories(mono SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_target_properties(mono
        PROPERTIES
        IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/mono-2.0-sgen.lib
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/mono-2.0-sgen.dll
        )