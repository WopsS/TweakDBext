#pragma once

#include "BaseSection.hpp"

class Queries : public BaseSection
{
public:
    using BaseSection::BaseSection;

    bool Load();

private:
    bool LoadQuery();
};
