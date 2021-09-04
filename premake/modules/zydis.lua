project("Zydis")
    targetdir(paths.build("libs"))

    kind("StaticLib")
    language("C")

    includedirs(
    {
        paths.deps("renhook", "deps", "zydis", "include"),
        paths.deps("renhook", "deps", "zydis", "src"),
        prj.includes("Zycore")
    })

    files(
    {
        paths.deps("renhook", "deps", "zydis", "src", "**.c"),
        paths.deps("renhook", "deps", "zydis", "include", "**.h")
    })

    links(
    {
        "Zycore"
    })
