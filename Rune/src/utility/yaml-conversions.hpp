#pragma once

#include <yaml-cpp/yaml.h>

#include <glm/ext/vector_float3.hpp>

namespace YAML
{
    template <>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node{};
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
}