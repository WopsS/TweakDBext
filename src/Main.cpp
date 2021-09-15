#include "stdafx.hpp"
#include "Hooks.hpp"
#include "Utils.hpp"

#include <RED4ext/Scripting/Natives/Generated/Color.hpp>
#include <RED4ext/Scripting/Natives/Generated/Vector3.hpp>
#include <iostream>

BOOL APIENTRY DllMain(HMODULE aModule, DWORD aReason, LPVOID aReserved)
{
    switch (aReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(aModule);

        Utils::CreateLogger();
        Hooks::Attach();

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        Hooks::Detach();
        spdlog::shutdown();

        break;
    }
    }

    return TRUE;
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Load(RED4ext::PluginHandle aHandle, const RED4ext::IRED4ext* aInterface)
{
    //{
    //    auto cacheDir = rootDir / "r6" / "cache";
    //    std::cout << "asd" << std::endl;
    //    //Sleep(5000);
    //    auto file = cacheDir / "tweakdb_delta.bin";

    //    TweakDB tweakDB(file);
    //    tweakDB.Load();
    //}

    auto db = RED4ext::TweakDB::Get();

    {
        RED4ext::TweakDBID id("WolvenKit.SomeBoolTrue");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeBoolTrue=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<bool>();
            std::cout << ", value=" << std::boolalpha << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeBoolFalse");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeBoolFalse=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<bool>();
            std::cout << ", value=" << std::boolalpha << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.AnotherBoolTrue");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.AnotherBoolTrue=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<bool>();
            std::cout << ", value=" << std::boolalpha << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeInt32");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeInt32=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<int32_t>();
            std::cout << ", value=" << std::dec << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeFloat");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeFloat=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<float>();
            std::cout << ", value=" << std::dec << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeCName");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeCName=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<RED4ext::CName>();
            std::cout << ", hash=" << std::showbase << std::hex << value->hash << ", text=" << value->ToString();
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeString");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeString=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<RED4ext::CString>();
            std::cout << ", value=" << std::dec << value->c_str();
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeColor");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeColor=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<RED4ext::Color>();
            std::cout << ", Red=" << std::dec << (int)value->Red << ", Green=" << std::dec << (int)value->Green
                      << ", Blue=" << std::dec << (int)value->Blue << ", Alpha=" << std::dec << (int)value->Alpha;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeVector3");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeVector3=" << std::showbase << std::hex << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<RED4ext::Vector3>();
            std::cout << ", x=" << std::dec << value->X << ", y=" << std::dec << value->Y << ", z=" << std::dec
                      << value->Z;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("WolvenKit.SomeArray<Bool>");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "WolvenKit.SomeArray<Bool>=" << std::showbase << std::hex << (uintptr_t)flatValue << std::endl;
    }

    {
        RED4ext::TweakDBID id("Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse");
        auto flatValue = db->GetFlatValue(id);
        std::cout << "Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse=" << std::showbase << std::hex
                  << (uintptr_t)flatValue;
        if (flatValue)
        {
            auto value = flatValue->GetValue<float>();
            std::cout << ", value=" << std::dec << *value;
        }

        std::cout << std::endl;
    }

    {
        RED4ext::TweakDBID id("VehicleTPP_DefaultParams");
        auto addr = db->GetRecord(id);
        std::cout << "VehicleTPP_DefaultParams=" << std::showbase << std::hex << (uintptr_t)addr.GetPtr() <<
        std::endl;
    }

    {
        RED4ext::TweakDBID id("Camera.VehicleTPP_DefaultParams");
        auto addr = db->GetRecord(id);
        std::cout << "Camera.VehicleTPP_DefaultParams=" << std::showbase << std::hex << (uintptr_t)addr.GetPtr() <<
        std::endl;
    }

    /*auto engine = RED4ext::CGameEngine::Get();
    auto rtti = RED4ext::CRTTISystem::Get();

    auto tdbClass = rtti->GetClass("TDB");
    auto tdb = engine->framework->gameInstance->GetInstance(tdbClass);*/

    RED4ext::TweakDBID id("Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse");
    auto result = 0.0f;
    RED4ext::ExecuteGlobalFunction("TDB::GetFloat;TweakDBID", &result, id);

    spdlog::trace("Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse={}", result);

    {
        RED4ext::TweakDBID id("Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse");
        auto flatValue = db->GetFlatValue(id);
        if (flatValue)
        {
            auto value = flatValue->GetValue<float>();
            spdlog::trace("Camera.VehicleTPP_DefaultParams.autoCenterStartTimeMouse={}", *value);
        }

        std::cout << std::endl;
    }

    return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Unload()
{
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"TweakDBext";
    aInfo->author = L"WopsS";
    aInfo->version = RED4EXT_SEMVER(1, 1, 1);
    aInfo->runtime = RED4EXT_RUNTIME_LATEST;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}
