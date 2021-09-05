#include "stdafx.hpp"
#include "Records.hpp"
#include "FileStructs.hpp"

bool Records::Load()
{
    if (!BaseSection::Load())
    {
        return false;
    }

    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return false;
    }

    auto db = RED4ext::TweakDB::Get();
    auto types = CollectRecordTypes();

    for (uint32_t i = 0; i < count; i++)
    {
        RecordInfo recordInfo;
        if (!m_reader->ReadWriteEx(&recordInfo))
        {
            return false;
        }

        auto iter = types.find(recordInfo.type);
        if (iter == types.end())
        {
            spdlog::warn("No type found, hash={:#x}", recordInfo.type);
            continue;
        }

        auto record = db->GetRecord(recordInfo.id);
        if (!record)
        {
            auto type = iter->second;
            if (!db->CreateRecord(recordInfo.id, type))
            {
                spdlog::warn("Could not create record for type '{}'", type->name.ToString());
            }
        }
        else
        {
            db->UpdateRecord(recordInfo.id);
        }
    }

    return true;
}

std::unordered_map<uint32_t, RED4ext::CClass*> Records::CollectRecordTypes()
{
    auto rtti = RED4ext::CRTTISystem::Get();
    auto base = rtti->GetClass("gamedataTweakDBRecord");
    if (!base)
    {
        spdlog::error("Could not get 'gamedataTweakDBRecord' class from RTTI");
        return {};
    }

    std::unordered_map<uint32_t, RED4ext::CClass*> result;
    rtti->types.ForEach([base, &result](RED4ext::CName aName, RED4ext::CBaseRTTIType* aType) {
        constexpr auto SEED = 0x5EEDBA5E;

        auto typeId = aType->GetType();
        if (typeId == RED4ext::ERTTIType::Class)
        {
            auto cls = static_cast<RED4ext::CClass*>(aType);
            if (cls->IsA(base))
            {
                /*
                 * Types stored in TweakDB files are using Murmur3_32 and the names are not the same as native ones,
                 * these names do not have "gamedata" prefix and "_Record" suffix, remove them and calcualte hash.
                 */

                auto rtti = RED4ext::CRTTISystem::Get();

                // Script names should not have "gamedata" prefix.
                RED4ext::CName scriptName = rtti->ConvertNativeToScriptName(cls->name);
                std::string_view name = scriptName.ToString();

                // Only making sure here.
                constexpr std::string_view PREFIX = "gamedata";
                auto pos = name.find(PREFIX);
                if (pos != std::string_view::npos)
                {
                    name.remove_prefix(PREFIX.size());
                }

                // Remove suffix.
                constexpr std::string_view SUFFIX = "_Record";
                pos = name.rfind(SUFFIX);
                if (pos != std::string_view::npos)
                {
                    name.remove_suffix(name.size() - pos);
                }

                // Everything should be corrent now, calculate hash.
                auto hash = RED4ext::Murmur3_32(reinterpret_cast<const uint8_t*>(name.data()), name.length(), SEED);
                result.emplace(hash, cls);
            }
        }
    });

    return result;
}
