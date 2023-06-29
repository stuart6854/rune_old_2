CPMAddPackage("gh:gabime/spdlog#v1.11.0")
CPMAddPackage("gh:g-truc/glm#0.9.9.8")
CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.8
        OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "GLFW_INSTALL OFF"
        "GLFW_USE_WAYLAND ON"
)

CPMAddPackage(
        NAME AudioFile
        GITHUB_REPOSITORY adamstark/AudioFile
        GIT_TAG 1.1.1
        SYSTEM TRUE
        OPTIONS
        "BUILD_TESTS OFF"
        "BUILD_EXAMPLES OFF"
)
CPMAddPackage(
        NAME openal-soft
        GITHUB_REPOSITORY kcat/openal-soft
        GIT_TAG 1.23.1
        SYSTEM TRUE
        OPTIONS
        "ALSOFT_UTILS OFF"
        "ALSOFT_NO_CONFIG_UTIL ON"
        "ALSOFT_EXAMPLES OFF"
        "ALSOFT_INSTALL OFF"
)
CPMAddPackage(
        NAME entt
        GITHUB_REPOSITORY skypjack/entt
        GIT_TAG v3.12.2
)
