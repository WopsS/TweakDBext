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
        spdlog::warn("Could not read types count");
        return {};
    }

    std::vector<RED4ext::CBaseRTTIType*> result(count);

    spdlog::trace("Reading type(s), count={}", count);

    auto rtti = RED4ext::CRTTISystem::Get();
    for (uint32_t i = 0; i < count; i++)
    {
        FlatType type;
        if (!m_reader->ReadWriteEx(&type))
        {
            spdlog::warn("Could not the flat type info");
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

    spdlog::trace("Type(s) read, count={}", count);
    return result;
}

std::vector<std::unique_ptr<Flats::Storage>> Flats::ReadValues(RED4ext::CBaseRTTIType* aType)
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        spdlog::warn("Could not read values count");
        return {};
    }

    auto typeName = aType->GetName();
    spdlog::trace("Reading value(s), count={}, type={}", count, typeName.ToString());

    std::vector<std::unique_ptr<Storage>> result(count);
    for (uint32_t i = 0; i < count; i++)
    {
        result[i] = std::make_unique<Storage>(typeName);
        if (!result[i]->type)
        {
            spdlog::error("The value's type '{}' is nullptr, this should not happen, index={}", typeName.ToString(), i);
            return {};
        }

        spdlog::trace("Unserializing a value, index={}", i);
        aType->Unserialize(m_reader, result[i]->memory, 0);
        spdlog::trace("Unserialized the value, index={}", i);
    }

    spdlog::trace("Value(s) read, count={}, type={}", count, typeName.ToString());

    return result;
}

bool Flats::LoadFlats(const std::vector<std::unique_ptr<Storage>>& aValues)
{
    uint32_t count;
    if (!m_reader->ReadWriteEx(&count))
    {
        spdlog::warn("Could not read flats count");
        return false;
    }

    /*
     * Our own custom flat pool, values might be duplicated in the game's pool, but at least this will be prevented in
     * the same database.
     *
     * Remarks: Used as <valueIndex, flat offset>.
     */
    std::unordered_map<uint32_t, int32_t> flatsPool;

    spdlog::trace("Reading flat(s), count={}", count);

    auto db = RED4ext::TweakDB::Get();
    for (uint32_t i = 0; i < count; i++)
    {
        FlatInfo flatInfo;
        if (!m_reader->ReadWriteEx(&flatInfo))
        {
            spdlog::warn("Could not flat info");
            return false;
        }

        spdlog::trace("Processing flat, flat_id={:#x}", flatInfo.id);

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
            spdlog::trace("Checking if flat exist, flat_id={:#x}", flatInfo.id);

            /*
             * Going to do a read lock, based on Sombra's research "lock_guard" is used for add/remove, "shared_lock"
             * for reads.
             */
            std::shared_lock<RED4ext::SharedMutex> _(db->mutex00);

            auto flatIt = db->flats.Find(flatInfo.id);
            if (flatIt != db->flats.End())
            {
                spdlog::trace("Flat exist, trying to changing offset, flat_id={:#x}", flatInfo.id);

                // ID already exist, try to change its offset.
                if (flatIt->ToTDBOffset() != offset)
                {
                    spdlog::trace("Offset is different, changing it, flat_id={:#x}", flatInfo.id);

                    auto flatValue = db->GetFlatValue(*flatIt);
                    auto currValue = flatValue->GetValue();

                    // Change the offset only if it is the same type.
                    if (currValue.type == value->type)
                    {
                        spdlog::trace("Flat offset changed, old={:#x}, new={:#x}", flatIt->ToTDBOffset(), offset);
                        flatIt->SetTDBOffset(offset);
                    }
                }
                else
                {
                    spdlog::trace("New offset is equal with the old one, not changing offset, flat_id={:#x}", flatInfo.id);
                }

                isNewFlat = false;
            }
        }

        if (isNewFlat)
        {
            spdlog::trace("Adding new flat, flat_id={:#x}", flatInfo.id);

            flatInfo.id.SetTDBOffset(offset);
            db->AddFlat(flatInfo.id);

            spdlog::trace("New flat added, flat_id={:#x}", flatInfo.id);
        }

        spdlog::trace("Flat processed, flat_id={:#x}", flatInfo.id);
    }

    spdlog::trace("Flat(s) read, count={}", count);
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

    type->Construct(memory);
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
