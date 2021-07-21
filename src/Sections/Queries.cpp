#include "stdafx.hpp"
#include "Queries.hpp"
#include "FileStructs.hpp"

bool Queries::Load()
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

    for (uint32_t i = 0; i < count; i++)
    {
        if (!LoadQuery())
        {
            return false;
        }
    }

    return true;
}

bool Queries::LoadQuery()
{
    QueryInfo info;
    if (!m_reader->ReadWriteEx(&info))
    {
        return false;
    }

    if (info.count == 0)
    {
        return true;
    }

    RED4ext::DynArray<RED4ext::TweakDBID> query;

    auto db = RED4ext::TweakDB::Get();
    if (db->HasQuery(info.id))
    {
        query = db->Query(info.id);
    }

    for (uint32_t i = 0; i < info.count; i++)
    {
        RED4ext::TweakDBID queryId;
        if (!m_reader->ReadWriteEx(&queryId))
        {
            return false;
        }

        query.PushBack(std::move(queryId));
    }

    if (db->HasQuery(info.id))
    {
        db->ReplaceQuery(info.id, std::move(query));
    }
    else
    {
        db->AddQuery(info.id, std::move(query));
    }

    return true;
}
