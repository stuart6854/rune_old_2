include("Dependencies.cmake")

add_subdirectory(RHI)

add_library(Rune STATIC 
	src/rune/core/engine.cpp
	src/rune/debug/log.cpp
	src/rune/events/events.cpp
	src/rune/platform/glfw/platform_glfw.cpp
	# src/rune/rendering/rhi/shader_compile.cpp
	# src/platform/vk/type_conversion_vk.cpp
	# src/platform/vk/instance_vk.cpp
	# src/platform/vk/device_vk.cpp
	# src/platform/vk/surface_vk.cpp
	# src/platform/vk/command_list_vk.cpp
	# src/platform/vk/fence_vk.cpp
	# src/platform/vk/image_vk.cpp
	# src/platform/vk/render_target_vk.cpp
	# src/platform/vk/shader_program_vk.cpp
	# src/platform/vk/vertex_input_format_vk.cpp
	# src/platform/vk/pipeline_state_vk.cpp
	# src/platform/vk/buffer_vk.cpp
	# src/platform/vk/_vk.cpp
)

target_include_directories(Rune 
	PRIVATE src
	PUBLIC include
)

target_link_libraries(Rune 
	PRIVATE spdlog::spdlog glfw
	PUBLIC glm::glm Rune_RHI
)

target_compile_definitions(Rune 
	PRIVATE 
		VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
	PUBLIC
		$<$<CONFIG:Debug>:RUNE_DEBUG>
		$<$<CONFIG:RelWithDebInfo>:RUNE_RELEASE>
		$<$<CONFIG:Release>:RUNE_DIST>
)

if(WIN32)
	target_compile_definitions(Rune 
		PUBLIC RUNE_PLATFORM_WINDOWS
	)
endif()

set(RUNE_GRAPHICS_API "Vulkan" CACHE STRING "What graphics API to compile with (Vulkan, D3D12, etc.).")
if(${RUNE_GRAPHICS_API} STREQUAL "Vulkan")
	message(STATUS "Rune is using Vulkan graphics.")
	target_compile_definitions(Rune PRIVATE RUNE_GRAPHICS_VULKAN)
elseif(${RUNE_GRAPHICS_API} STREQUAL "D3D12")
	message(STATUS "Rune is using D3D12 graphics.")
	target_compile_definitions(Rune PRIVATE RUNE_GRAPHICS_D3D12)
endif()