workspace "Rune"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    startproject "Sandbox"

outputdir = "%{cfg.system}-%{cfg.buildcfg}"

group "Engine"
    include "Rune"

group "Runtimes"
    include "Sandbox"

group ""