set(TARGET_NAME sandbox)

file(GLOB_RECURSE ${TARGET_NAME}_SOURCES "src/*.cpp")
add_executable(${TARGET_NAME} WIN32 ${${TARGET_NAME}_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_NAME}> $<INSTALL_INTERFACE:include/${TARGET_NAME}>)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

if (MSVC)
    # warning level 4 and all warnings as error
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

target_link_libraries(${TARGET_NAME} PRIVATE rune)

set_target_properties(${TARGET_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}" # .lib/.a
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}" # .so/.dylib
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}" # .exe/.dll
        )

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        #        COMMAND ${CMAKE_COMMAND} -E echo "runtime dlls = $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>" # Print required runtime dlls
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
        COMMAND_EXPAND_LISTS
        )