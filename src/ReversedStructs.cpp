#include "stdafx.hpp"
#include "ReversedStructs.hpp"

extern size_t g_Unk01_ctor;

Unk01::Unk01(RED4ext::CName aTypeName)
{
    using func_t = Unk01*(*)(Unk01* aThis, RED4ext::CName aTypeName);
    RED4ext::REDfunc<func_t> func(g_Unk01_ctor);
    func(this, aTypeName);
}

Unk01::~Unk01()
{
    type->Destroy(memory);
    if (unk20)
    {
        auto db = RED4ext::TweakDB::Get();
        auto allocator = db->unk118.GetAllocator();

        allocator->Free(memory);
    }
}
