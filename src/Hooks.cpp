#include "stdafx.hpp"
#include "Hooks.hpp"
#include "TweakDB.hpp"
#include "Utils.hpp"

namespace
{
constexpr auto TweakDB_Load_Addr = 0x140C42FA0 - RED4ext ::Addresses::ImageBase;

void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2);
renhook::prologue_hook<decltype(&_TweakDB_Load)> TweakDB_Load;

void _TweakDB_Load(RED4ext::TweakDB* aThis, RED4ext::CString& a2)
{
    TweakDB_Load(aThis, a2);

    auto rootDir = Utils::GetRootDir();
    auto tweakdbsDir = rootDir / "r6" / "tweakdbs";

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
    auto addr = TweakDB_Load_Addr + reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    new (&TweakDB_Load) renhook::prologue_hook<decltype(&_TweakDB_Load)>(addr, &_TweakDB_Load);

    TweakDB_Load.attach();
}

void Hooks::Detach()
{
    TweakDB_Load.detach();
}
