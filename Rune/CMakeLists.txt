include("Dependencies.cmake")

add_library(Rune STATIC 
	src/rune/core/engine.cpp
	src/rune/debug/log.cpp
	src/rune/events/events.cpp
	src/rune/platform/glfw/platform_glfw.cpp
	src/rune/rendering/rhi/shader_compile.cpp
	src/platform/vk/type_conversion_vk.cpp
	src/platform/vk/instance_vk.cpp
	src/platform/vk/device_vk.cpp
	src/platform/vk/surface_vk.cpp
	src/platform/vk/command_list_vk.cpp
	src/platform/vk/fence_vk.cpp
	src/platform/vk/image_vk.cpp
	src/platform/vk/render_target_vk.cpp
	src/platform/vk/shader_program_vk.cpp
	src/platform/vk/vertex_input_format_vk.cpp
	src/platform/vk/pipeline_state_vk.cpp
	src/platform/vk/buffer_vk.cpp
	# src/platform/vk/_vk.cpp
)

target_include_directories(Rune 
	PRIVATE src
	PUBLIC include
)

target_link_libraries(Rune 
	PRIVATE spdlog::spdlog glfw Vulkan::Vulkan unofficial::VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp unofficial::shaderc::shaderc
	PUBLIC glm::glm
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
		PRIVATE VK_USE_PLATFORM_WIN32_KHR
		PUBLIC RUNE_PLATFORM_WINDOWS
	)
endif()