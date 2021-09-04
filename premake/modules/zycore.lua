project("Zycore")
    targetdir(paths.build("libs"))

    kind("StaticLib")
    language("C")

    includedirs(
    {
        paths.deps("zyantific"),
        paths.deps("renhook", "deps", "zydis", "dependencies", "zycore", "include")
    })

    files(
    {
        paths.deps("renhook", "deps", "zydis", "dependencies", "zycore", "src", "**.c"),
        paths.deps("renhook", "deps", "zydis", "dependencies", "zycore", "include", "**.h")
    })
