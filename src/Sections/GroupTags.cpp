#include "stdafx.hpp"
#include "GroupTags.hpp"
#include "FileStructs.hpp"

bool GroupTags::Load()
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
    for (uint32_t i = 0; i < count; i++)
    {
        GroupInfo info;
        if (!m_reader->ReadWriteEx(&info))
        {
            return false;
        }

        if (db->HasGroupTag(info.id))
        {
            db->ReplaceGroupTag(info.id, info.tag);
        }
        else
        {
            db->AddGroupTag(info.id, info.tag);
        }
    }

    return true;
}
