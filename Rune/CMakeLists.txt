set(TARGET_NAME rune)

file(GLOB_RECURSE ${TARGET_NAME}_SOURCES "src/*.cpp")
add_library(${TARGET_NAME} STATIC ${${TARGET_NAME}_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_NAME}> $<INSTALL_INTERFACE:include/${TARGET_NAME}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4 and all warnings as error
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()