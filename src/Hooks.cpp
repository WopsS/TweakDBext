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
    spdlog::trace("Executing real function");
    TweakDB_Load(aThis, a2);
    spdlog::trace("Real function executed");

    auto rootDir = Utils::GetRootDir();
    spdlog::trace(L"Root directory is '{}'", rootDir.c_str());

    auto tweakdbsDir = rootDir / "r6" / "tweakdbs";

    spdlog::trace("Reading directory...");
    for (const auto& entry : std::filesystem::recursive_directory_iterator(tweakdbsDir))
    {
        spdlog::trace(L"Checking entry...");

        const auto& path = entry.path();
        spdlog::trace(L"Path for entry is '{}'", path.c_str());

        auto extension = entry.path().extension();
        spdlog::trace(L"Extension is '{}'", extension.c_str());

        if (extension == L".bin")
        {
            try
            {
                TweakDB tweakDB(path);
                tweakDB.Load();
            }
            catch (const std::exception& ex)
            {
                spdlog::warn(L"An exception occured while trying to load '{}' database", path.c_str());
                spdlog::warn(ex.what());
            }
            catch (...)
            {
                spdlog::warn(L"An error occured while trying to load '{}' database", path.c_str());
            }
        }
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
    spdlog::trace("Attaching hook");
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
    spdlog::trace("Hook attached");
}

void Hooks::Detach()
{
    DetourTransactionBegin();
    DetourThreadsUpdater _;

    DetourDetach(&TweakDB_Load, _TweakDB_Load);
    DetourTransactionCommit();
}
