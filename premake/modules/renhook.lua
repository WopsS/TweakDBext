project("RenHook")
    targetdir(paths.build("libs"))

    kind("StaticLib")
    language("C++")

    filter({ "configurations:Debug" })
        optimize("On")
        runtime("Debug")

    filter({})

    includedirs(
    {
        paths.deps("renhook", "src"),
        prj.includes("Zydis")
    })

    files(
    {
        paths.deps("renhook", "src", "**.cpp"),
        paths.deps("renhook", "src", "**.hpp")
    })

    links(
    {
        prj.links("Zydis")
    })
