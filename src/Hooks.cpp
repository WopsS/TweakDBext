#include "stdafx.hpp"
#include "Hooks.hpp"
#include "Addresses.hpp"
#include "TweakDB.hpp"
#include "Utils.hpp"

namespace
{
void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2);
renhook::prologue_hook<decltype(&_TweakDB_Load)> TweakDB_Load;

void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2)
{
    spdlog::trace("Calling the original function");
    TweakDB_Load(aThis, a2);
    spdlog::trace("Call succeeded");

    auto rootDir = Utils::GetRootDir();
    auto tweakdbsDir = rootDir / "r6" / "tweakdbs";

    spdlog::trace(L"Loading additional TweakDBs, dir={}", tweakdbsDir.c_str());
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
    spdlog::trace("Updating records");
    std::lock_guard<RED4ext::SharedMutex> _(aThis->mutex01);

    std::vector<RED4ext::TweakDBID> records;
    aThis->recordsByID.for_each([&records](const auto& aId, auto& aHandle) { records.push_back(aId); });

    for (auto record : records)
    {
        if (!aThis->UpdateRecord(record))
        {
            spdlog::warn("Failed to update record, record_id={:#x}", record);
        }
    }

    spdlog::trace("Records updated, count={}", records.size());
}
} // namespace

void Hooks::Attach()
{
    auto addr = Addresses::TweakDB_Load + reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    new (&TweakDB_Load) renhook::prologue_hook<decltype(&_TweakDB_Load)>(addr, &_TweakDB_Load);

    spdlog::trace("Attaching hook, addr={:#x}", addr);
    TweakDB_Load.attach();
    spdlog::trace("Hook attached", addr);
}

void Hooks::Detach()
{
    spdlog::trace("Detaching hook");
    TweakDB_Load.detach();
    spdlog::trace("Hook detached");
}
