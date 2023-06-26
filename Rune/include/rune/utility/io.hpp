#pragma once

#include <string>
#include <expected>
#include <filesystem>

namespace rune::utility::io
{
    auto read_string(const std::filesystem::path& filename) -> std::expected<std::string, std::string>;
    auto read_bytes(const std::filesystem::path& filename) -> std::expected<std::vector<std::uint8_t>, std::string>;
}