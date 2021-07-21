#pragma once

#include "BaseSection.hpp"
#include "ReversedStructs.hpp"

class GroupTags : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();
};
