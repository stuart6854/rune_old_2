include(Dependencies.cmake)

set(ASSETLIB_TARGET_NAME assetlib)

file(GLOB_RECURSE ${ASSETLIB_TARGET_NAME}_SOURCES "src/*.cpp")
add_library(${ASSETLIB_TARGET_NAME} STATIC ${${ASSETLIB_TARGET_NAME}_SOURCES})

target_include_directories(${ASSETLIB_TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/rune/${ASSETLIB_TARGET_NAME}> $<INSTALL_INTERFACE:include/${ASSETLIB_TARGET_NAME}>)
target_include_directories(${ASSETLIB_TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${ASSETLIB_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

target_link_libraries(${ASSETLIB_TARGET_NAME} PUBLIC cxxopts::cxxopts glm::glm assimp::assimp)

set_target_properties(${ASSETLIB_TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4 and all warnings as error
    target_compile_options(${ASSETLIB_TARGET_NAME} PRIVATE /W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    target_compile_options(${ASSETLIB_TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${ASSETLIB_TARGET_NAME} PUBLIC RUNE_BUILD_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(${ASSETLIB_TARGET_NAME} PUBLIC RUNE_BUILD_RELEASE)
endif ()

# Command Line Tool App
set(ASSETLIB_CMD_TARGET_NAME rune_assetlib_cmd)
add_executable(${ASSETLIB_CMD_TARGET_NAME} src/main.cpp)
target_link_libraries(${ASSETLIB_CMD_TARGET_NAME} PRIVATE ${ASSETLIB_TARGET_NAME})