require("premake", ">=5.0.0-alpha16")

paths = require("extensions/paths")
prj = require("extensions/project")

workspace("TweakDBext")
    location("projects")
    startproject("TweakDBext")

    architecture("x86_64")
    configurations({ "Debug", "Release" })
    characterset("Unicode")
    symbols("Full")

    cppdialect("C++20")
    systemversion("latest")

    defines(
    {
        "WINVER=0x0601",
        "_WIN32_WINNT=0x0601",
        "WIN32_LEAN_AND_MEAN",
        "_CRT_SECURE_NO_WARNINGS"
    })

    filter({ "configurations:Release" })
        optimize("On")

    filter({})

    group("Dependencies")
        include(paths.modules("Detours"))
        include(paths.modules("spdlog"))

    group("")
        include(paths.src())
