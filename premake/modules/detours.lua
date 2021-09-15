project("Detours")
    targetdir(paths.build("libs"))

    kind("StaticLib")
    language("C++")

    includedirs(
    {
        paths.deps("detours", "src")
    })

    files(
    {
        paths.deps("detours", "src", "**.cpp"),
        paths.deps("detours", "src", "**.h"),
    })

    removefiles(
    {
        paths.deps("detours", "src", "uimports.cpp")
    })
