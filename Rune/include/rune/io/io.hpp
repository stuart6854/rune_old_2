// #TODO: Write functions
// #TODO: Streamed Read functions - eg. read_bytes(filename, size_t start, size_t size)

#pragma once

#include "common_internal.hpp"

#include <string>
#include <fstream>
#include <istream>
#include <expected>
#include <filesystem>

namespace rune::io
{
#pragma region Read-Only

    auto read_string(const std::filesystem::path& filename) -> std::expected<std::string, std::string>;

    template <typename T>
    auto read_binary(const std::filesystem::path& filename) -> std::expected<std::vector<T>, std::string>
    {
        auto file = std::ifstream(filename, std::ios::binary);
        if (!file)
        {
            return std::unexpected("Failed to open file: " + filename.string());
        }

        file.seekg(0, std::ios::end);
        const auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<T> buffer(fileSize / sizeof(T));
        file.read((char*)buffer.data(), fileSize);

        return buffer;
    }

#pragma endregion

#pragma region Write

#pragma endregion
}