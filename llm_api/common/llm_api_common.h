#pragma once

#include <cstdint>
#include <type_traits>
#include <charconv> // std::to_chars

#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <memory>

#define LLM_API_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type<ENUMTYPE>::type)a) | ((std::underlying_type<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator |= (ENUMTYPE& a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((std::underlying_type<ENUMTYPE>::type&)a) |= ((std::underlying_type<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type<ENUMTYPE>::type)a) & ((std::underlying_type<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator &= (ENUMTYPE& a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((std::underlying_type<ENUMTYPE>::type&)a) &= ((std::underlying_type<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) noexcept { return ENUMTYPE(~((std::underlying_type<ENUMTYPE>::type)a)); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((std::underlying_type<ENUMTYPE>::type)a) ^ ((std::underlying_type<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator ^= (ENUMTYPE& a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((std::underlying_type<ENUMTYPE>::type&)a) ^= ((std::underlying_type<ENUMTYPE>::type)b)); }

static_assert(sizeof(size_t) == 8);

#if defined(LLM_API_PLATFORM_WINDOWS_DEF_)
#define LLM_API_PLATFORM_WINDOWS 1
#else
#define LLM_API_PLATFORM_WINDOWS 0
#endif

#if defined(LLM_API_PLATFORM_UNIX_DEF_)
#define LLM_API_PLATFORM_UNIX 1
#else
#define LLM_API_PLATFORM_UNIX 0
#endif

#ifndef LLM_API_USE_ARENA_ALLOCATOR
#define LLM_API_USE_ARENA_ALLOCATOR 0 // TODO should be on all platforms, for some reason (to_chars(float) doesnt work on gcc)
#endif

#ifndef LLM_API_USE_SIMDSTRING
#define LLM_API_USE_SIMDSTRING LLM_API_PLATFORM_WINDOWS // TODO should be on all platforms, for some reason (to_chars(float) doesnt work on gcc)
#endif

#if LLM_API_USE_ARENA_ALLOCATOR
#include <llm_api/common/deps/arena-allocator/arena_alloc.h>
#define LLM_API_STL_NAMESPACE ::arena
namespace llm_api
{
template <typename T>
using allocator = ::arena::Allocator<T>;
}
#else
#define LLM_API_STL_NAMESPACE ::std
namespace llm_api
{
template <typename T>
using allocator = ::std::allocator<T>;
}
#endif

#if LLM_API_USE_SIMDSTRING
#include <llm_api/common/deps/SIMDString/SIMDString.h>
#endif

#define LLM_API_CFG_DEBUG       1
#define LLM_API_CFG_DEV         2
#define LLM_API_CFG_RELEASE     3
#ifndef LLM_API_CONFIG // set this value as PUBLIC define for llm_api_common cmake target according to LLM_API_CFG_* definitions above
#define LLM_API_CONFIG          LLM_API_CFG_RELEASE
#endif
#define LLM_API_DEBUG_LAYER     ((LLM_API_CONFIG == LLM_API_CFG_DEBUG) || (LLM_API_CONFIG == LLM_API_CFG_DEV))

#if LLM_API_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

#include <llm_api/common/log_category.h>

#if LLM_API_DEBUG_LAYER

#define LLM_API_LOGGER_ENABLE LLM_API_PLATFORM_WINDOWS
#define LLM_API_ASSERT_ENABLE 1

#define LLM_API_DEBUG_LAYER_ON_CODE(...) __VA_ARGS__
#define LLM_API_DEBUG_LAYER_OFF_CODE(...)
#else

#define LLM_API_LOGGER_ENABLE 0
#define LLM_API_ASSERT_ENABLE 0

#define LLM_API_DEBUG_LAYER_ON_CODE(...)
#define LLM_API_DEBUG_LAYER_OFF_CODE(...) __VA_ARGS__
#endif

#if LLM_API_LOGGER_ENABLE
#include <llm_api/common/logger.h>

#define LLM_API_INFO(lc, fmt, ...)      ::llm_api::g_Logger.Log(lc, llm_api::LogLevel::Info, fmt, __VA_ARGS__)
#define LLM_API_WARNING(lc, fmt, ...)   ::llm_api::g_Logger.Log(lc, llm_api::LogLevel::Warning, fmt, __VA_ARGS__)
#define LLM_API_ERROR(lc, fmt, ...)     ::llm_api::g_Logger.Log(lc, llm_api::LogLevel::Error, fmt, __VA_ARGS__)
#else
#define LLM_API_INFO(...)
#define LLM_API_WARNING(...)
#define LLM_API_ERROR(...)
#endif

#if LLM_API_ASSERT_ENABLE
#include <llm_api/common/deps/debugbreak/debugbreak.h>

#if LLM_API_PLATFORM_WINDOWS
#include <debugapi.h> // IsDebuggerPresent
#define DEBUGGER_PRESENT_CHECK IsDebuggerPresent()
#else
#define DEBUGGER_PRESENT_CHECK true
#endif

#define LLM_API_ASSERT(cond) \
if ((!(cond)) && DEBUGGER_PRESENT_CHECK)\
{\
	debug_break();\
}
#else
#define LLM_API_ASSERT(cond) ((void)(cond))
#endif

namespace llm_api
{
using uint8     = uint8_t;
using uint16    = uint16_t;
using uint32    = uint32_t;
using uint64    = uint64_t;

enum class Result : uint32
{
    Failure = 0U,
    Success = 1U,
};

enum class LlmVendor : uint32
{
    OPENAI = 0U,

    COUNT,
};

enum class FeatureFlags : uint64
{
    TOKEN_COUNT = 1ULL << 0,
    COST_APPROX = 1ULL << 1
};
LLM_API_DEFINE_ENUM_FLAG_OPERATORS(::llm_api::FeatureFlags)

// specialize this in every LLM backend
template <LlmVendor Vendor>
struct llm_text_model_enum_for_vendor {};

template <LlmVendor Vendor>
using llm_text_model_enum_for_vendor_t = typename llm_text_model_enum_for_vendor<Vendor>::model_enum;

template <LlmVendor Vendor>
struct llm_image_model_enum_for_vendor {};

template <LlmVendor Vendor>
using llm_image_model_enum_for_vendor_t = typename llm_image_model_enum_for_vendor<Vendor>::model_enum;

using llm_enum_underlying_t = uint32;
struct LlmTextModel
{
    LlmVendor vendor;
    llm_enum_underlying_t llm;

    template <LlmVendor Vendor>
    LLM_API_DEBUG_LAYER_OFF_CODE(constexpr) llm_text_model_enum_for_vendor_t<Vendor> GetModelAs() const
    {
        LLM_API_ASSERT(Vendor == vendor);
        return (llm_text_model_enum_for_vendor_t<Vendor>) llm;
    }

    auto operator<=>(const LlmTextModel&) const = default;
};

#include <llm_api/common/llm_api_token_count.inl>

using string_view = std::string_view;

#if LLM_API_USE_SIMDSTRING
enum : size_t { SIMDSTRING_INTERNAL_SIZE = 128ULL };
using string = SIMDString<(size_t) SIMDSTRING_INTERNAL_SIZE, ::llm_api::allocator<char>>;
#else
using string = std::basic_string<char, std::char_traits<char>, ::llm_api::allocator<char>>;
#endif

#if LLM_API_USE_ARENA_ALLOCATOR || LLM_API_USE_SIMDSTRING
using ::getline;

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
string to_string(T x)
{
#if LLM_API_USE_SIMDSTRING
    return ::to_string<(size_t) SIMDSTRING_INTERNAL_SIZE, ::llm_api::allocator<char>>(x);
#else
    return std::to_string(x);
#endif
}
template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
string to_string(T x, int precision = 5)
{
    constexpr size_t BufLen = 32ULL;
    char buf[BufLen];
    LLM_API_ASSERT(precision > 0);
    LLM_API_ASSERT((size_t) (precision + 2) <= BufLen);

    auto [end, ec] = std::to_chars(buf, buf + BufLen, x, std::chars_format::fixed, precision);
    LLM_API_ASSERT(ec == std::errc());
    if (ec != std::errc())
        return string{};

    return string(buf, end);
}
#else
using ::std::getline;
using ::std::to_string;

auto to_string(auto x)
{
    return ::std::to_string(x);
}
template <typename T>
::std::string to_string(const T& x, int)
{
    return ::std::to_string(x);
}
#endif

template <typename T>
using vector = LLM_API_STL_NAMESPACE::vector<T>;
template <typename T, typename H = std::hash<T>>
using unordered_set = LLM_API_STL_NAMESPACE::unordered_set<T, H>;
template <typename K, typename V, typename H = std::hash<K>>
using unordered_map = LLM_API_STL_NAMESPACE::unordered_map<K, V, H>;

template <typename T>
struct default_delete
{
    constexpr default_delete() = default;

    template <class T2, std::enable_if_t<std::is_convertible_v<T2*, T*>, int> = 0>
    default_delete(const default_delete<T2>&) {}

    void operator()(T* ptr) const
    {
        static_assert(0ULL < sizeof(T), "Can't delete an incomplete type");
        ptr->~T();
        allocator<T> alctr;
        alctr.deallocate(ptr, 1ULL);
    }
};

template <typename T>
struct default_delete<T[]>
{
    constexpr default_delete() = default;

    template <typename U, std::enable_if_t<std::is_convertible_v<U(*)[], U(*)[]>, int> = 0>
    default_delete(const default_delete<U[]>&) {}

    template <typename U, std::enable_if_t<std::is_convertible_v<U(*)[], T(*)[]>, int> = 0>
    void operator()(U* ptr) const
    {
        static_assert(0ULL < sizeof(U), "Can't delete an incomplete type");
        //delete[] ptr; // TODO
        LLM_API_ASSERT(0);
    }
};

template <typename T>
using unique_ptr = ::std::unique_ptr<T, ::llm_api::default_delete<T>>;

template <typename T, typename... Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
[[nodiscard]] ::llm_api::unique_ptr<T> make_unique(Args&&... args)
{
    allocator<T> alctr;
    T* const retval = alctr.allocate(1ULL);
    return ::llm_api::unique_ptr<T>(new (retval) T(std::forward<Args>(args)...));
}

enum EntryPointType : uint8
{
    ChatCompletion,
    TextCompletion,
    ImageGeneration,
    ImageEdit,
    ImageVariation
};

using apikey_t = string_view;
using modelname_t = string_view;
using apientrypoint_t = string_view;

struct Message
{
    string_view role;
    string content;

    Message() = default;
    Message(string_view _role, string&& msg)
        : role(_role)
        , content(msg)
    {

    }

    bool empty() const { return content.empty(); }

    static Message System(string&& msg)
    {
        return Message("system", std::move(msg));
    }
    static Message User(string&& msg)
    {
        return Message("user", std::move(msg));
    }
    static Message Assistant(string&& msg)
    {
        return Message("assistant", std::move(msg));
    }
};

struct DialogueFragment
{
    Message user_msg;
    Message assistant_msg;
};
using message_array = vector<Message>;

using image_url_t = string;
using image_data_vector = vector<uint8>;
struct ImageExtent
{
    uint16 width;
    uint16 height;
};
struct Image
{
    string name;
    ImageExtent extent;
    image_data_vector data;

    const uint8* GetData() const { return data.data(); }
    size_t GetByteSize() const { return data.size(); }
};


Result ChatML(apientrypoint_t ep, apikey_t apikey, modelname_t model, message_array&& msgs, Message& output);

Result Completion_ImageResponse(apientrypoint_t ep, apikey_t apikey, modelname_t model, string_view prompt, ImageExtent size, image_url_t& output);
Result Completion_ImageResponse(apientrypoint_t ep, apikey_t apikey, modelname_t model, string_view prompt, ImageExtent size, Image& output);

class Interface
{
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;

public:
    Interface() = default;
    virtual ~Interface() = default;

    virtual bool IsInitialized() = 0;
    virtual Result Initialize(apikey_t apikey) = 0;
    virtual Result Finalize() = 0;

    virtual bool IsFeatureSupported(FeatureFlags feature) = 0;

    virtual modelname_t TextModelToString(llm_enum_underlying_t model) = 0;
    virtual llm_enum_underlying_t TextModelStringToEnum(modelname_t modelname) = 0;

    virtual Result ChatML(llm_enum_underlying_t model, message_array&& msgs, Message& output) = 0;

    virtual size_t ComputeTokenCount(llm_enum_underlying_t model, const string& txt) { return (size_t) 0; } // default impl when not supported
    TokenCount ComputeTokenCount(llm_enum_underlying_t model, const Message& txt);

    virtual double TokenCountToUSDCost(llm_enum_underlying_t model, const TokenCount& token_count) { return -1.0; } // default impl when not supported
    double MessageToUSDCost(llm_enum_underlying_t model, const Message& txt);
};

}