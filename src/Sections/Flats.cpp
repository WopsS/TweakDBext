#include "stdafx.hpp"
#include "Flats.hpp"
#include "FileStructs.hpp"
#include "Utils.hpp"

bool Flats::Load()
{
    if (!BaseSection::Load())
    {
        return false;
    }

    auto types = ReadTypes();
    for (auto type : types)
    {
        auto values = ReadValues(type);
        if (!LoadFlats(values))
        {
            return false;
        }
    }

    return true;
}

std::vector<RED4ext::IRTTIType*> Flats::ReadTypes()
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return {};
    }

    std::vector<RED4ext::IRTTIType*> result(count);

    auto rtti = RED4ext::CRTTISystem::Get();
    for (uint32_t i = 0; i < count; i++)
    {
        FlatType type;
        if (!m_reader->ReadWriteEx(&type))
        {
            break;
        }

        result[i] = rtti->GetType(type.name);
        if (!result[i])
        {
            auto typeName = type.name.ToString();
            spdlog::warn("Flat type not found, type='{}', hash={:#x}", typeName, type.name);
        }
    }

    return result;
}

std::vector<std::unique_ptr<Unk01>> Flats::ReadValues(RED4ext::IRTTIType* aType)
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return {};
    }

    std::vector<std::unique_ptr<Unk01>> result(count);
    for (uint32_t i = 0; i < count; i++)
    {
        RED4ext::CName name;
        aType->GetName(name);

        result[i] = std::make_unique<Unk01>(name);
        auto& unk = result[i];

        aType->Unserialize(m_reader, unk->memory, 0);
    }

    return result;
}

bool Flats::LoadFlats(const std::vector<std::unique_ptr<Unk01>>& aValues)
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return false;
    }

    /*
     * Our own custom flat pool, values might be duplicated in the game's pool, but at least this will be prevented in
     * the same database.
     *
     * Remarks: Used as <valueIndex, flat offset>.
     */
    std::unordered_map<uint32_t, int32_t> flatsPool;

    auto db = RED4ext::TweakDB::Get();
    for (uint32_t i = 0; i < count; i++)
    {
        FlatInfo flatInfo;
        if (!m_reader->ReadWriteEx(&flatInfo))
        {
            return false;
        }

        int32_t offset;
        auto& value = aValues.at(flatInfo.valueIndex);

        auto it = flatsPool.find(flatInfo.valueIndex);
        if (it != flatsPool.end())
        {
            offset = it->second;
        }
        else
        {
            RED4ext::CStackType stackType;
            stackType.type = value->type;
            stackType.value = value->memory;

            offset = db->CreateFlatValue(stackType);
            flatsPool.emplace(flatInfo.valueIndex, offset);
        }

        flatInfo.id.SetTDBOffset(offset);
        db->AddFlat(flatInfo.id);
    }

    return true;
}
