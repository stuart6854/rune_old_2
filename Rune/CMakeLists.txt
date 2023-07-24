include("Dependencies.cmake")

add_library(Rune STATIC 
	src/rune/core/engine.cpp
	src/rune/debug/log.cpp
	src/rune/events/events.cpp
	src/rune/platform/glfw/platform_glfw.cpp
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