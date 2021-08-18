# TweakDB format

| Section           | Subsection[size]  | Offset                | Size      | Members                       | Note                                  |
|-------------------|-------------------|-----------------------|-----------|-------------------------------|---------------------------------------|
| **Root**          | Header            | `0x00`                | `0x20`    | `magic : uint32`              | -                                     |
|                   |                   |                       |           | `blob_version : uint32`       | -                                     |
|                   |                   |                       |           | `parser_version : uint32`     | -                                     |
|                   |                   |                       |           | `records_checksum : uint32`   | -                                     |
|                   |                   |                       |           | `flats_offset : uint32`       | -                                     |
|                   |                   |                       |           | `records_offset : uint32`     | -                                     |
|                   |                   |                       |           | `queries_offset : uint32`     | -                                     |
|                   |                   |                       |           | `group_tags_offset : uint32`  | -                                     |
| **Flats**         |                   | `flats_offset`        | `dynamic` |                               | -                                     |
|                   | Types Header      |                       | `0x4`     | `count : uint32`              | -                                     |
|                   | Types[count]      |                       | `0xC`     | `name : CName`                | -                                     |
|                   |                   |                       |           | `values_count : uint32`       | How many values are with the type.    |
|                   | Values Header     |                       | `0x4`     | `count : uint32`              | -                                     |
|                   | Values[count]     |                       | `dynamic` | `value members`               | Depends on the value type.            |
|                   |  \|-Flats Header  |                       | `0x4`     | `count : uint32`              | -                                     |
|                   |  \|-Flats[count]  |                       | `0xC`     | `id : TweakDBID`              | -                                     |
|                   |                   |                       |           | `value_index : uint32`        | -                                     |
| **Records**       |                   | `records_offset`      | `dynamic` |                               | -                                     |
|                   | Header            |                       | `0x4`     | `count : uint32`              | -                                     |
|                   | Records[count]    |                       | `0xC`     | `id : TweakDBID`              | -                                     |
|                   |                   |                       |           | `type : uint32`               | Murmur3 hash of type's name.          |
| **Queries**       |                   | `queries_offset`      | `dynamic` |                               | -                                     |
|                   | Header            |                       | `0x4`     | `count : uint32`              | -                                     |
|                   | Queries[count]    |                       | `0xC`     | `id : TweakDBID`              | -                                     |
|                   |                   |                       |           | `unks_count : uint32`         | -                                     |
|                   |                   |                       |           | `unk[unks_count] : TweakDBID` | -                                     |
| **Group Tags**    |                   | `group_tags_offset`   | `dynamic` |                               | -                                     |
|                   | Header            |                       | `0x4`     | `count : uint32`              | -                                     |
|                   | Tags[count]       |                       | `0xC`     | `id : TweakDBID`              | -                                     |
|                   |                   |                       |           | `type : uint32`               | -                                     |
