#pragma once

#include "BaseSection.hpp"
#include "ReversedStructs.hpp"

class Queries : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();

private:
    bool LoadQuery();
};
