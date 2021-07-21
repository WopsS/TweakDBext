#pragma once

#include "BaseSection.hpp"

class Records : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();

private:
    std::unordered_map<uint32_t, RED4ext::CClass*> CollectRecordTypes();
};
