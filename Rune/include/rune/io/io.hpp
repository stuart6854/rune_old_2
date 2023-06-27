// #TODO: Write functions
// #TODO: Streamed Read functions - eg. read_bytes(filename, size_t start, size_t size)

#pragma once

#include <string>
#include <expected>
#include <filesystem>

namespace rune::io
{
#pragma region Read-Only

    auto read_string(const std::filesystem::path& filename) -> std::expected<std::string, std::string>;
    auto read_bytes(const std::filesystem::path& filename) -> std::expected<std::vector<std::uint8_t>, std::string>;

#pragma endregion

#pragma region Write

#pragma endregion
}