include(Dependencies.cmake)
add_subdirectory(libs/submodules)
if (NOT WIN32)
    find_package(OpenAL CONFIG REQUIRED)
endif ()

set(TARGET_NAME rune)

file(GLOB_RECURSE ${TARGET_NAME}_SOURCES "src/*.cpp")
add_library(${TARGET_NAME} STATIC ${${TARGET_NAME}_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_NAME}> $<INSTALL_INTERFACE:include/${TARGET_NAME}>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

target_link_libraries(${TARGET_NAME} PUBLIC spdlog glm::glm glfw AudioFile OpenAL::OpenAL gfx EnTT::EnTT)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4 and all warnings as error
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${TARGET_NAME} PUBLIC RUNE_BUILD_DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(${TARGET_NAME} PUBLIC RUNE_BUILD_RELEASE)
elseif (CMAKE_BUILD_TYPE MATCHES Distro)
    target_compile_definitions(${TARGET_NAME} PUBLIC RUNE_BUILD_DISTRO)
endif ()