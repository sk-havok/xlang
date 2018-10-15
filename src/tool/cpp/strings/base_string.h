
namespace winrt::impl
{
    inline constexpr hresult error_ok{ 0 }; // S_OK
    inline constexpr hresult error_false{ 1 }; // S_FALSE
    inline constexpr hresult error_fail{ static_cast<hresult>(0x80004005) }; // E_FAIL
    inline constexpr hresult error_access_denied{ static_cast<hresult>(0x80070005) }; // E_ACCESSDENIED
    inline constexpr hresult error_wrong_thread{ static_cast<hresult>(0x8001010E) }; // RPC_E_WRONG_THREAD
    inline constexpr hresult error_not_implemented{ static_cast<hresult>(0x80004001) }; // E_NOTIMPL
    inline constexpr hresult error_invalid_argument{ static_cast<hresult>(0x80070057) }; // E_INVALIDARG
    inline constexpr hresult error_out_of_bounds{ static_cast<hresult>(0x8000000B) }; // E_BOUNDS
    inline constexpr hresult error_no_interface{ static_cast<hresult>(0x80004002) }; // E_NOINTERFACE
    inline constexpr hresult error_class_not_available{ static_cast<hresult>(0x80040111) }; // CLASS_E_CLASSNOTAVAILABLE
    inline constexpr hresult error_changed_state{ static_cast<hresult>(0x8000000C) }; // E_CHANGED_STATE
    inline constexpr hresult error_illegal_method_call{ static_cast<hresult>(0x8000000E) }; // E_ILLEGAL_METHOD_CALL
    inline constexpr hresult error_illegal_state_change{ static_cast<hresult>(0x8000000D) }; // E_ILLEGAL_STATE_CHANGE
    inline constexpr hresult error_illegal_delegate_assignment{ static_cast<hresult>(0x80000018) }; // E_ILLEGAL_DELEGATE_ASSIGNMENT
    inline constexpr hresult error_canceled{ static_cast<hresult>(0x800704C7) }; // HRESULT_FROM_WIN32(ERROR_CANCELLED)
    inline constexpr hresult error_bad_alloc{ static_cast<hresult>(0x8007000E) }; // E_OUTOFMEMORY
    inline constexpr hresult error_not_initialized{ static_cast<hresult>(0x800401F0) }; // CO_E_NOTINITIALIZED

    inline void* duplicate_string(void* other)
    {
        void* result = nullptr;
        check_hresult(WINRT_WindowsDuplicateString(other, &result));
        return result;
    }

    inline void* create_string(wchar_t const* value, uint32_t const length)
    {
        void* result = nullptr;
        check_hresult(WINRT_WindowsCreateString(value, length, &result));
        return result;
    }

    inline bool embedded_null(void* value) noexcept
    {
        int32_t result = 0;
        WINRT_VERIFY_(error_ok, WINRT_WindowsStringHasEmbeddedNull(value, &result));
        return 0 != result;
    }

    struct hstring_traits
    {
        using type = void*;

        static void close(type value) noexcept
        {
            WINRT_VERIFY_(error_ok, WINRT_WindowsDeleteString(value));
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };
}

WINRT_EXPORT namespace winrt
{
    struct hstring
    {
        using value_type = wchar_t;
        using size_type = uint32_t;
        using const_reference = value_type const&;
        using pointer = value_type*;
        using const_pointer = value_type const*;
        using const_iterator = const_pointer;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        hstring() noexcept = default;

        hstring(construct_from_abi_t, void* ptr) noexcept : m_handle(ptr)
        {
        }

        hstring(hstring const& value) :
            m_handle(impl::duplicate_string(value.m_handle.get()))
        {}

        hstring& operator=(hstring const& value)
        {
            m_handle.attach(impl::duplicate_string(value.m_handle.get()));
            return*this;
        }

        hstring(hstring&&) noexcept = default;
        hstring& operator=(hstring&&) = default;
        hstring(std::nullptr_t) = delete;

        hstring(std::initializer_list<wchar_t> value) :
            hstring(value.begin(), static_cast<uint32_t>(value.size()))
        {}

        hstring(wchar_t const* value) :
            hstring(std::wstring_view(value))
        {}

        hstring(wchar_t const* value, size_type size) :
            m_handle(impl::create_string(value, size))
        {}

        explicit hstring(std::wstring_view const& value) :
            hstring(value.data(), static_cast<size_type>(value.size()))
        {}

        hstring& operator=(std::wstring_view const& value)
        {
            return *this = hstring{ value };
        }

        hstring& operator=(wchar_t const* const value)
        {
            return *this = hstring{ value };
        }

        hstring& operator=(std::initializer_list<wchar_t> value)
        {
            return *this = hstring{ value };
        }

        void clear() noexcept
        {
            m_handle.close();
        }

        operator std::wstring_view() const noexcept
        {
            uint32_t size;
            wchar_t const* data = WINRT_WindowsGetStringRawBuffer(m_handle.get(), &size);
            return std::wstring_view(data, size);
        }

        const_reference operator[](size_type pos) const noexcept
        {
            WINRT_ASSERT(pos < size());
            return*(begin() + pos);
        }

        const_reference front() const noexcept
        {
            WINRT_ASSERT(!empty());
            return*begin();
        }

        const_reference back() const noexcept
        {
            WINRT_ASSERT(!empty());
            return*(end() - 1);
        }

        const_pointer data() const noexcept
        {
            return begin();
        }

        const_pointer c_str() const noexcept
        {
            return begin();
        }

        const_iterator begin() const noexcept
        {
            return WINRT_WindowsGetStringRawBuffer(m_handle.get(), nullptr);
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        const_iterator end() const noexcept
        {
            uint32_t length = 0;
            const_pointer buffer = WINRT_WindowsGetStringRawBuffer(m_handle.get(), &length);
            return buffer + length;
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return rbegin();
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const noexcept
        {
            return rend();
        }

        bool empty() const noexcept
        {
            return !m_handle;
        }

        size_type size() const noexcept
        {
            return WINRT_WindowsGetStringLen(m_handle.get());
        }

        friend void swap(hstring& left, hstring& right) noexcept
        {
            swap(left.m_handle, right.m_handle);
        }

    private:

        handle_type<impl::hstring_traits> m_handle;
    };

    inline void* get_abi(hstring const& object) noexcept
    {
        return *(void**)(&object);
    }

    inline void** put_abi(hstring& object) noexcept
    {
        WINRT_ASSERT(get_abi(object) == nullptr);
        return reinterpret_cast<void**>(&object);
    }

    inline void attach_abi(hstring& object, void* value) noexcept
    {
        object.clear();
        *put_abi(object) = value;
    }

    inline void* detach_abi(hstring& object) noexcept
    {
        void* temp = get_abi(object);
        *reinterpret_cast<void**>(&object) = nullptr;
        return temp;
    }

    inline void* detach_abi(hstring&& object) noexcept
    {
        return detach_abi(object);
    }

    inline void copy_from_abi(hstring& object, void* value)
    {
        attach_abi(object, impl::duplicate_string(value));
    }

    inline void copy_to_abi(hstring const& object, void*& value)
    {
        WINRT_ASSERT(value == nullptr);
        value = impl::duplicate_string(get_abi(object));
    }

    inline void* detach_abi(std::wstring_view const& value)
    {
        return impl::create_string(value.data(), static_cast<uint32_t>(value.size()));
    }

    inline void* detach_abi(wchar_t const* const value)
    {
        return impl::create_string(value, static_cast<uint32_t>(wcslen(value)));
    }
}

namespace winrt::impl
{
    template <> struct abi<hstring>
    {
        using type = void*;
    };

    template <> struct name<hstring>
    {
        static constexpr auto & value{ L"String" };
        static constexpr auto & data{ "string" };
    };

    template <> struct category<hstring>
    {
        using type = basic_category;
    };

    // Temporary workaround to support locale-independent numeric formatting
    // until C++17's to_chars arrives
    struct locale_handle_traits
    {
        using type = _locale_t;

        static void close(type value) noexcept
        {
            _free_locale(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    inline _locale_t get_default_locale()
    {
        static handle_type<locale_handle_traits> locale_handle{ _create_locale(LC_ALL, "C") };
        return locale_handle.get();
    }

    struct hstring_builder
    {
        hstring_builder(hstring_builder const&) = delete;
        hstring_builder& operator=(hstring_builder const&) = delete;

        explicit hstring_builder(uint32_t const size)
        {
            check_hresult(WINRT_WindowsPreallocateStringBuffer(size, &m_data, &m_buffer));
        }

        ~hstring_builder() noexcept
        {
            if (m_buffer != nullptr)
            {
                WINRT_VERIFY_(error_ok, WINRT_WindowsDeleteStringBuffer(m_buffer));
            }
        }

        wchar_t* data() noexcept
        {
            WINRT_ASSERT(m_buffer != nullptr);
            return m_data;
        }

        hstring to_hstring()
        {
            WINRT_ASSERT(m_buffer != nullptr);
            hstring result;
            check_hresult(WINRT_WindowsPromoteStringBuffer(m_buffer, put_abi(result)));
            m_buffer = nullptr;
            return result;
        }

    private:

        wchar_t* m_data{ nullptr };
        void* m_buffer{ nullptr };
    };
}

WINRT_EXPORT namespace winrt
{
    inline bool embedded_null(hstring const& value) noexcept
    {
        return impl::embedded_null(get_abi(value));
    }

    inline hstring to_hstring(uint8_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%hhu", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(int8_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%hhd", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(uint16_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%hu", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(int16_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%hd", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(uint32_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%I32u", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(int32_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%I32d", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(uint64_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%I64u", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(int64_t value)
    {
        wchar_t buffer[32];
        swprintf_s(buffer, L"%I64d", value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(float value)
    {
        wchar_t buffer[32];
        _swprintf_s_l(buffer, std::size(buffer), L"%G", impl::get_default_locale(), value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(double value)
    {
        wchar_t buffer[32];
        _swprintf_s_l(buffer, std::size(buffer), L"%G", impl::get_default_locale(), value);
        return hstring{ buffer };
    }

    inline hstring to_hstring(char16_t value)
    {
        wchar_t buffer[2] = { value, 0 };
        return hstring{ buffer };
    }

    inline hstring to_hstring(hstring const& value) noexcept
    {
        return value;
    }

    template <typename T, typename = std::enable_if_t<std::is_same_v<T, bool>>>
    hstring to_hstring(T const value)
    {
        if (value)
        {
            return hstring{ L"true" };
        }
        else
        {
            return hstring{ L"false" };
        }
    }

    inline hstring to_hstring(guid const& value)
    {
        wchar_t buffer[40];
        //{00000000-0000-0000-0000-000000000000}
        swprintf_s(buffer, L"{%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}",
            value.Data1, value.Data2, value.Data3, value.Data4[0], value.Data4[1],
            value.Data4[2], value.Data4[3], value.Data4[4], value.Data4[5], value.Data4[6], value.Data4[7]);
        return hstring{ buffer };
    }

    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T, std::string_view>>>
    hstring to_hstring(T const& value)
    {
        std::string_view const view(value);
        int const size = WINRT_MultiByteToWideChar(65001 /*CP_UTF8*/, 0, view.data(), static_cast<int32_t>(view.size()), nullptr, 0);

        if (size == 0)
        {
            return{};
        }

        impl::hstring_builder result(size);
        WINRT_VERIFY_(size, WINRT_MultiByteToWideChar(65001 /*CP_UTF8*/, 0, view.data(), static_cast<int32_t>(view.size()), result.data(), size));
        return result.to_hstring();
    }

    inline std::string to_string(std::wstring_view value)
    {
        int const size = WINRT_WideCharToMultiByte(65001 /*CP_UTF8*/, 0, value.data(), static_cast<int32_t>(value.size()), nullptr, 0, nullptr, nullptr);

        if (size == 0)
        {
            return{};
        }

        std::string result(size, '?');
        WINRT_VERIFY_(size, WINRT_WideCharToMultiByte(65001 /*CP_UTF8*/, 0, value.data(), static_cast<int32_t>(value.size()), result.data(), size, nullptr, nullptr));
        return result;
    }
}
