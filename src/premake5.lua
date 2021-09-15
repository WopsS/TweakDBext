project("TweakDBext")
    targetdir(paths.build("bin"))

    kind("SharedLib")
    language("C++")
    pchheader("stdafx.hpp")
    pchsource("stdafx.cpp")

    defines(
    {
        prj.defines("spdlog")
    })

    includedirs(
    {
        ".",
        prj.includes("Detours"),
        prj.includes("spdlog"),
        paths.deps("red4ext.sdk", "include")
    })

    files(
    {
        "**.cpp",
        "**.hpp"
    })

    links(
    {
        prj.links("Detours"),
        prj.links("spdlog")
    })
