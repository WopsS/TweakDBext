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
            TweakDB tweakDB(path);
            tweakDB.Load();
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
