#pragma once

namespace llm_api
{
enum class LogLevel
{
    Info = 0,
    Warning,
    Error,

    COUNT
};

inline const char* LogLevelToString(LogLevel ll)
{
#define _ADD_CASE(level) case LogLevel:: level: return #level
    switch (ll)
    {
        _ADD_CASE(Info);
        _ADD_CASE(Warning);
        _ADD_CASE(Error);
        default: return "";
    }
#undef _ADD_CASE
}

struct LogCategory
{
    enum : size_t { MAX_LEN = 32ULL };

    template <size_t N>
    constexpr LogCategory(const char(&c)[N]) : m_c(c), m_len(N - 1ULL)
    {
        static_assert(N <= (MAX_LEN + 1ULL), "Log category name must be at most 32 characters!");
    }

    constexpr explicit operator const char* () const { return m_c; }
    constexpr size_t length() const { return m_len; }

    const char* const m_c;
    const size_t m_len;
};
}