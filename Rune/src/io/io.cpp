#include "io/io.hpp"

#include <fstream>
#include <istream>
#include <sstream>

namespace rune::io
{
    auto read_string(const std::filesystem::path& filename) -> std::expected<std::string, std::string>
    {
        auto file = std::ifstream(filename, std::ios::in);
        if (!file)
        {
            return std::unexpected("Failed to open file: " + filename.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    auto read_bytes(const std::filesystem::path& filename) -> std::expected<std::vector<std::uint8_t>, std::string>
    {
        auto file = std::ifstream("file.txt", std::ios::binary);
        if (!file)
        {
            return std::unexpected("Failed to open file: " + filename.string());
        }

        file.seekg(0, std::ios::end);
        const auto fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<std::uint8_t> buffer{};
        buffer.reserve(fileSize);
        buffer.insert(buffer.begin(), std::istream_iterator<std::uint8_t>(file), std::istream_iterator<std::uint8_t>());

        return buffer;
    }
}