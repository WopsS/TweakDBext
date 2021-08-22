#pragma once

#include "BaseSection.hpp"

class GroupTags : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();
};
