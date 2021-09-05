#include "stdafx.hpp"
#include "Flats.hpp"
#include "FileStructs.hpp"
#include "Utils.hpp"

namespace
{
RED4ext::Memory::GMPL_TDB_LoaderAllocator tdbLoaderAllocator;
}

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

std::vector<RED4ext::CBaseRTTIType*> Flats::ReadTypes()
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return {};
    }

    std::vector<RED4ext::CBaseRTTIType*> result(count);

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
            if (!typeName)
            {
                typeName = "unknown";
            }

            spdlog::error("Flat type not found, type='{}', hash={:#x}", typeName, type.name);
            return {};
        }
    }

    return result;
}

std::vector<std::unique_ptr<Flats::Storage>> Flats::ReadValues(RED4ext::CBaseRTTIType* aType)
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        return {};
    }

    std::vector<std::unique_ptr<Storage>> result(count);
    for (uint32_t i = 0; i < count; i++)
    {
        auto name = aType->GetName();

        result[i] = std::make_unique<Storage>(name);
        if (!result[i]->type)
        {
            return {};
        }

        aType->Unserialize(m_reader, result[i]->memory, 0);
    }

    return result;
}

bool Flats::LoadFlats(const std::vector<std::unique_ptr<Storage>>& aValues)
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

        int32_t offset = 0;
        auto& value = aValues.at(flatInfo.valueIndex);

        auto it = flatsPool.find(flatInfo.valueIndex);
        if (it != flatsPool.end())
        {
            offset = it->second;
        }
        else
        {
            // I am not sure if offset = 0 is the "does not exist", might be value at offset 0 so I'm using a boolean.
            bool isSameValue = false;

            /*
             * Does the key exist and has a value? If it does then do a check if the user is not trying to replace the
             * key's value with the same value (yes, it is dumb but who knows), this should prevent adding duplicate
             * flats for the same key.
             */
            auto currFlatValue = db->GetFlatValue(flatInfo.id);
            if (currFlatValue)
            {
                auto currValue = currFlatValue->GetValue();
                if (currValue.type == value->type && currValue.type->IsEqual(currValue.value, value->memory))
                {
                    offset = currFlatValue->ToTDBOffset();
                    isSameValue = true;
                }
            }

            if (!isSameValue)
            {
                RED4ext::CStackType stackType;
                stackType.type = value->type;
                stackType.value = value->memory;

                offset = db->CreateFlatValue(stackType);
                if (offset == -1)
                {
                    spdlog::warn("Failed to create a new flat value, flat_id={:#x}", flatInfo.id);
                }
            }

            flatsPool.emplace(flatInfo.valueIndex, offset);
        }

        bool isNewFlat = true;
        {
            /*
             * Going to do a read lock, based on Sombra's research "lock_guard" is used for add/remove, "shared_lock"
             * for reads.
             */
            std::shared_lock<RED4ext::SharedMutex> _(db->mutex00);

            auto flatIt = db->flats.Find(flatInfo.id);
            if (flatIt != db->flats.End())
            {
                // ID already exist, try to change its offset.
                if (flatIt->ToTDBOffset() != offset)
                {
                    auto flatValue = db->GetFlatValue(*flatIt);
                    auto currValue = flatValue->GetValue();

                    // Change the offset only if it is the same type.
                    if (currValue.type == value->type)
                    {
                        flatIt->SetTDBOffset(offset);
                    }
                }

                isNewFlat = false;
            }
        }

        if (isNewFlat)
        {
            flatInfo.id.SetTDBOffset(offset);
            db->AddFlat(flatInfo.id);
        }
    }

    return true;
}

Flats::Storage::Storage(RED4ext::CName aType)
    : type(nullptr)
    , memory(nullptr)
{
    auto rtti = RED4ext::CRTTISystem::Get();
    type = rtti->GetType(aType);
    if (!type)
    {
        return;
    }

    auto size = type->GetSize();
    auto alignment = type->GetAlignment();

    // Here they store the value inline if it size is less than 0x10 and alignment less than 0x8, but we don't care
    // about that and will request it form allocator anyway.
    auto allocResult = tdbLoaderAllocator.AllocAligned(size, alignment);
    if (allocResult.memory)
    {
        memory = allocResult.memory;
    }

    type->Init(memory);
    if (type->GetType() == RED4ext::ERTTIType::Array)
    {
        new (memory) RED4ext::Memory::GMPL_TDB_LoaderAllocator();
    }
}

Flats::Storage::~Storage()
{
    if (memory)
    {
        tdbLoaderAllocator.Free(memory);
    }
}
