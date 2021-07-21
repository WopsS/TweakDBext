#include "stdafx.hpp"
#include "BaseSection.hpp"

BaseSection::BaseSection(RED4ext::BaseStream* aReader, uint32_t aOffset)
    : m_reader(aReader)
    , m_offset(aOffset)
{
}

bool BaseSection::Load()
{
    if (!m_reader->Seek(m_offset))
    {
        return false;
    }

    return true;
}
