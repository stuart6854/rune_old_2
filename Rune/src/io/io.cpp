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

}