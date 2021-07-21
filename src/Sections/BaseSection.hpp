#pragma once

class BaseSection
{
public:
    BaseSection(RED4ext::BaseStream* aReader, uint32_t aOffset);
    ~BaseSection() = default;

    virtual bool Load();

protected:
    RED4ext::BaseStream* m_reader;

private:
    uint32_t m_offset;
};
