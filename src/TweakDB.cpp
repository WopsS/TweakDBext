#include "stdafx.hpp"
#include "TweakDB.hpp"
#include "IO/FileStream.hpp"
#include "Sections/Flats.hpp"
#include "Sections/GroupTags.hpp"
#include "Sections/Queries.hpp"
#include "Sections/Records.hpp"

TweakDB::TweakDB(const std::filesystem::path& aPath)
    : m_path(aPath)
{
}

bool TweakDB::Load()
{
    auto fileName = m_path.filename().stem();

    spdlog::debug(L"Processing TweakDB file '{}'", fileName.c_str());

    FileStream file(m_path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN);
    if (!file.IsOpen())
    {
        spdlog::warn(L"Could not open file, file_name={}, errno={:#x}", fileName.c_str(), GetLastError());
        return false;
    }

    auto fileSize = file.GetLength();
    RED4ext::Memory::EngineAllocator allocator;
    auto allocResult = allocator.AllocAligned(static_cast<uint32_t>(fileSize), 1);
    auto buffer = allocResult.memory;

    file.ReadWrite(buffer, static_cast<uint32_t>(fileSize));

    /*
     * Doing it this way because the game does it too.
     *
     * Someone might say "but why not using FileStream directly?", well I tried that first and apparently somewhere deep
     * in CClass::Unserialize it will call a function that will use stream's vtbl and the registers are not saved when
     * calling it, the FileStream is changing the registers and thus leading to crashes.
     *
     * We could implement our own memory stream, but too much hassle.
     */
    auto reader = allocator.Alloc<RED4ext::MemoryStream>();
    RED4ext::MemoryStream::Construct(reader, buffer, fileSize);

    FileHeader header;
    if (!reader->ReadWriteEx(&header))
    {
        spdlog::warn(L"Could not read the header, file_name={}, file_size={}", fileName.c_str(), fileSize);

        allocator.Free(reader);
        allocator.Free(&allocResult);

        return false;
    }

    if (header.magic != MAGIC)
    {
        spdlog::warn(L"Magic mismatched, file_name={}, found={:#x}, expected={:#x}", fileName.c_str(), header.magic,
                     MAGIC);

        allocator.Free(reader);
        allocator.Free(&allocResult);

        return false;
    }

    spdlog::debug(L"file_name={} blob={} parser={} flats_offset={:#x} records_offset={:#x} queries_offset={:#x} "
                  L"groupTags_offset={:#x}",
                  fileName.c_str(), header.blobVersion, header.parserVersion, header.offsets.flats,
                  header.offsets.records, header.offsets.queries, header.offsets.groupTags);


    Flats flats(reader, header.offsets.flats);
    flats.Load();

    Records records(reader, header.offsets.records);
    records.Load();

    Queries queries(reader, header.offsets.queries);
    queries.Load();

    GroupTags groupTags(reader, header.offsets.groupTags);
    groupTags.Load();

    allocator.Free(reader);
    allocator.Free(&allocResult);

    return true;
}
