#pragma once

#include "FileStructs.hpp"

class TweakDB
{
public:
    static constexpr uint32_t MAGIC = 0xBB1DB47;

    TweakDB(const std::filesystem::path& aPath);
    ~TweakDB() = default;

    bool Load();

private:
    std::filesystem::path m_path;
};
