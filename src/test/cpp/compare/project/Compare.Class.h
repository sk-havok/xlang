#pragma once
#include "Compare.Class.g.h"

namespace winrt::Compare::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        hstring Method();
    };
}
namespace winrt::Compare::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
