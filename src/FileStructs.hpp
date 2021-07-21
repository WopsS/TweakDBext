#pragma once

#pragma pack(push, 1)
struct FileHeader
{
    struct Offsets
    {
        uint32_t flats;     // 00
        uint32_t records;   // 04
        uint32_t queries;   // 08
        uint32_t groupTags; // 0C
    };
    RED4EXT_ASSERT_SIZE(Offsets, 0x10);
    RED4EXT_ASSERT_OFFSET(Offsets, flats, 0x0);
    RED4EXT_ASSERT_OFFSET(Offsets, records, 0x4);
    RED4EXT_ASSERT_OFFSET(Offsets, queries, 0x8);
    RED4EXT_ASSERT_OFFSET(Offsets, groupTags, 0xC);

    uint32_t magic;           // 00
    uint32_t blobVersion;     // 04
    uint32_t parserVersion;   // 08
    uint32_t recordsChecksum; // 0C
    Offsets offsets;          // 10
};
RED4EXT_ASSERT_SIZE(FileHeader, 0x20);
RED4EXT_ASSERT_OFFSET(FileHeader, magic, 0x0);
RED4EXT_ASSERT_OFFSET(FileHeader, blobVersion, 0x4);
RED4EXT_ASSERT_OFFSET(FileHeader, parserVersion, 0x8);
RED4EXT_ASSERT_OFFSET(FileHeader, recordsChecksum, 0xC);
RED4EXT_ASSERT_OFFSET(FileHeader, offsets, 0x10);

struct FlatType
{
    RED4ext::CName name;
    uint32_t unk8;
};
RED4EXT_ASSERT_SIZE(FlatType, 0xC);
RED4EXT_ASSERT_OFFSET(FlatType, name, 0x0);
RED4EXT_ASSERT_OFFSET(FlatType, unk8, 0x8);

struct FlatInfo
{
    RED4ext::TweakDBID id;
    uint32_t valueIndex;
};
RED4EXT_ASSERT_SIZE(FlatInfo, 0xC);
RED4EXT_ASSERT_OFFSET(FlatInfo, id, 0x0);
RED4EXT_ASSERT_OFFSET(FlatInfo, valueIndex, 0x8);

struct RecordInfo
{
    RED4ext::TweakDBID id;
    uint32_t type; // Murmur3
};
RED4EXT_ASSERT_SIZE(RecordInfo, 0xC);
RED4EXT_ASSERT_OFFSET(RecordInfo, id, 0x0);
RED4EXT_ASSERT_OFFSET(RecordInfo, type, 0x8);

struct QueryInfo
{
    RED4ext::TweakDBID id;
    uint32_t count;
};
RED4EXT_ASSERT_SIZE(QueryInfo, 0xC);
RED4EXT_ASSERT_OFFSET(QueryInfo, id, 0x0);
RED4EXT_ASSERT_OFFSET(QueryInfo, count, 0x8);

struct GroupInfo
{
    RED4ext::TweakDBID id;
    RED4ext::TweakDB::GroupTag tag;
};
RED4EXT_ASSERT_SIZE(GroupInfo, 0x9);
RED4EXT_ASSERT_OFFSET(GroupInfo, id, 0x0);
RED4EXT_ASSERT_OFFSET(GroupInfo, tag, 0x8);

#pragma pack(pop)
