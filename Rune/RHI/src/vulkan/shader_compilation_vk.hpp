#pragma once

#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace rune::rhi
{
    auto read_shader_source(const std::string& filename) -> std::string;

    auto compile_shader(const std::string& sourceCode,
                        vk::ShaderStageFlagBits stage,
                        bool debugInfo,
                        bool optimize,
                        const std::vector<std::string>& defines,
                        const char* filename) -> std::vector<std::uint8_t>;
}