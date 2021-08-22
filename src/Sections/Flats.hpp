#pragma once

#include "BaseSection.hpp"

class Flats : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();

private:
    struct Storage
    {
        Storage(RED4ext::CName aType);
        ~Storage();

        RED4ext::CBaseRTTIType* type;
        void* memory;
    };

    std::vector<RED4ext::CBaseRTTIType*> ReadTypes();
    std::vector<std::unique_ptr<Storage>> ReadValues(RED4ext::CBaseRTTIType* aType);
    bool LoadFlats(const std::vector<std::unique_ptr<Storage>>& aValues);
};
