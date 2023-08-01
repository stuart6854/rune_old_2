include("Dependencies.cmake")

add_library(Rune_RHI STATIC 
	src/vulkan/device_vk.cpp
	src/vulkan/resources_vk.cpp
	src/vulkan/type_conversions_vk.cpp
	src/rune/rhi/shader_compilation.cpp
)

target_include_directories(Rune_RHI 
	PRIVATE src
	PUBLIC include
)

target_link_libraries(Rune_RHI 
	PRIVATE Vulkan::Vulkan unofficial::VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp unofficial::shaderc::shaderc
	PUBLIC glm::glm
)

target_compile_definitions(Rune_RHI 
	PRIVATE 
		VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
)

if(WIN32)
	target_compile_definitions(Rune_RHI 
		PRIVATE VK_USE_PLATFORM_WIN32_KHR
	)
endif()