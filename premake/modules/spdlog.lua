project("spdlog")
    targetdir(paths.build("libs"))

    kind("StaticLib")
    language("C++")

    defines(
    {
        "SPDLOG_COMPILED_LIB",
        "SPDLOG_NO_THREAD_ID",
        "SPDLOG_WCHAR_FILENAMES",
        "SPDLOG_WCHAR_TO_UTF8_SUPPORT",
        "SPDLOG_DISABLE_DEFAULT_LOGGER"
    })

    includedirs(
    {
        paths.deps("spdlog", "include")
    })

    files(
    {
        paths.deps("spdlog", "src", "**.cpp"),
        paths.deps("spdlog", "include", "**.h"),
    })
