project "Sandbox"
    -- location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    debugdir "%{prj.location}"

    files
    {
        "src/**.cpp"
    }

    includedirs
    {
        "src/",
        "include/"
    }

    externalincludedirs
    {
        "../Rune/include"
        "../vendor/glm"
    }

    links
    {
        "../Rune/vendor/glfw/lib/glfw3_mt",
        "Rune"
    }

    flags
    {
        "FatalCompileWarnings"
    }

    warnings "High"

    externalanglebrackets "On"
    externalwarnings "Off"

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RUNE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        defines "RUNE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        defines "RUNE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        runtime "Release"
        defines "RUNE_DIST"
        optimize "On"

    filter ""