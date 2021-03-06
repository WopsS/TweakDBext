#include "stdafx.hpp"
#include "Hooks.hpp"
#include "Addresses.hpp"
#include "DetourThreadsUpdater.hpp"
#include "TweakDB.hpp"
#include "Utils.hpp"

namespace
{
void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2);
decltype(&_TweakDB_Load) TweakDB_Load;

void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2)
{
    TweakDB_Load(aThis, a2);

    auto rootDir = Utils::GetRootDir();
    auto tweakdbsDir = rootDir / "r6" / "tweakdbs";

    try
    {
        if (!std::filesystem::exists(tweakdbsDir))
        {
            std::filesystem::create_directories(tweakdbsDir);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(tweakdbsDir))
        {
            const auto& path = entry.path();
            if (entry.path().extension() == L".bin")
            {
                try
                {
                    TweakDB tweakDB(path);
                    tweakDB.Load();
                }
                catch (const std::exception& ex)
                {
                    spdlog::error(L"An exception occured while trying to load '{}' database", path.c_str());
                    spdlog::error(ex.what());
                }
                catch (...)
                {
                    spdlog::error(L"An unknown error occured while trying to load '{}' database", path.c_str());
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        spdlog::error(L"An exception occured while reading the directory '{}'", tweakdbsDir.c_str());
        spdlog::error(ex.what());
    }
    catch (...)
    {
        spdlog::error(L"An unknown error occured while reading the directory '{}'", tweakdbsDir.c_str());
    }

    // Doing some quick hack to update records until TweakDB writer is finalized.
    auto db = RED4ext::TweakDB::Get();
    std::lock_guard<RED4ext::SharedMutex> _(db->mutex01);

    std::vector<RED4ext::TweakDBID> records;
    db->recordsByID.for_each([&records](const auto& aId, auto& aHandle) { records.push_back(aId); });

    for (auto record : records)
    {
        if (!db->UpdateRecord(record))
        {
            spdlog::warn("Failed to update record, record_id={:#x}", record);
        }
    }
}
} // namespace

void Hooks::Attach()
{
    DetourTransactionBegin();
    DetourThreadsUpdater _;

    TweakDB_Load = reinterpret_cast<decltype(TweakDB_Load)>(Addresses::TweakDB_Load +
                                                            reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)));
    auto a = &_TweakDB_Load;
    auto result = DetourAttach(&TweakDB_Load, _TweakDB_Load);
    if (result != NO_ERROR)
    {
        auto message = fmt::format(L"Could not attach hook for main function, attach returned {}.\n\nProcess will exit "
                                   L"to prevent further problems.",
                                   result);
        MessageBox(nullptr, message.c_str(), L"RED4ext", MB_ICONERROR | MB_OK);
    }
    DetourTransactionCommit();
}

void Hooks::Detach()
{
    DetourTransactionBegin();
    DetourThreadsUpdater _;

    DetourDetach(&TweakDB_Load, _TweakDB_Load);
    DetourTransactionCommit();
}
