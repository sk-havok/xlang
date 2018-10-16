#include "pch.h"
#include "Class.h"

namespace winrt::Component::implementation
{
    Component::Class Class::ReturnObject()
    {
        throw hresult_not_implemented();
    }
    hstring Class::Method()
    {
        throw hresult_not_implemented();
    }
    hstring Class::Property()
    {
        throw hresult_not_implemented();
    }
    void Class::Property(hstring)
    {
        throw hresult_not_implemented();
    }
}
