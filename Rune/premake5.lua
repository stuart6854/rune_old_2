project "Rune"
    -- location "Rune"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.cpp",
        "src/**.hpp",
        "include/**.hpp"
    }

    includedirs
    {
        "include/",
        "src/"
    }

    externalincludedirs
    {
        "vendor/*/include",
        "../vendor/glm"
    }

    flags
    {
        "FatalCompileWarnings"
    }

    warnings "High"

    externalanglebrackets "On"
    externalwarnings "Off"

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "RUNE_PLATFORM_WINDOWS"
        }

        -- https://premake.github.io/docs/postbuildcommands/
        -- postbuildcommand
        -- {
            -- ("{COPY}")
        -- }
        
    filter "system:linux"

    filter "configurations:Debug"
        buildoptions "/MTd" 
        runtime "Debug"
        defines "RUNE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        buildoptions "/MT"
        runtime "Release"
        defines "RUNE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        buildoptions "/MT"
        runtime "Release"
        defines "RUNE_DIST"
        optimize "On"

    filter ""