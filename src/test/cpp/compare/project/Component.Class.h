#pragma once
#include "Component.Class.g.h"

namespace winrt::Component::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        Component::Class ReturnObject();
        hstring Method();
        hstring Property();
        void Property(hstring);
    };
}
namespace winrt::Component::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
