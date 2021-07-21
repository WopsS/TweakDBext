#include "stdafx.hpp"
#include "TweakDB.hpp"
#include "Utils.hpp"

// Offset of the ctor.
size_t g_Unk01_ctor;

RED4EXT_C_EXPORT bool RED4EXT_CALL Load(RED4ext::PluginHandle aHandle, const RED4ext::IRED4ext* aInterface)
{
    auto hooking = aInterface->GetHookingInterface();

    // Find Unk01's ctor.
    uint8_t pattern[] = {0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x6C, 0x24, 0x18, 0x48, 0x89, 0x74,
                         0x24, 0x20, 0x57, 0x48, 0x83, 0xEC, 0x40, 0x48, 0x8B, 0xDA, 0x48, 0x8B, 0xF9};
    g_Unk01_ctor =
        reinterpret_cast<size_t>(hooking->FindPattern(pattern, sizeof(pattern) / sizeof(pattern[0]), 0xCC, 8, 6)) -
        reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));

    if (!g_Unk01_ctor)
    {
        spdlog::error("Could not find Unk01's ctor");
        return false;
    }

    Utils::CreateLogger();

    auto rootDir = Utils::GetRootDir();
    auto cacheDir = rootDir / "r6" / "cache";

    {
        auto file = cacheDir / "tweakdb_modified.bin";

        TweakDB tweakDB(file);
        tweakDB.Load();
    }

    return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Unload()
{
    spdlog::shutdown();
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"TweakDBext";
    aInfo->author = L"WopsS";
    aInfo->version = RED4EXT_SEMVER(0, 1, 0);
    aInfo->runtime = RED4EXT_RUNTIME_LATEST;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}
