#pragma once

struct Unk01
{
    Unk01(RED4ext::CName aTypeName);
    ~Unk01();

    RED4ext::CBaseRTTIType* type;
    void* memory;
    int64_t unk10;
    int64_t unk18;
    int8_t unk20;
};
RED4EXT_ASSERT_SIZE(Unk01, 0x28);
RED4EXT_ASSERT_OFFSET(Unk01, type, 0x0);
RED4EXT_ASSERT_OFFSET(Unk01, memory, 0x8);
