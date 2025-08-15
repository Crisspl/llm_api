#pragma once

#include <llm_api/common/deps/debugbreak/debugbreak.h>
#include <llm_api/common/log_category.h>

namespace llm_api
{

struct Logger
{
    static const bool nobreakonerror;

    static void Log(const LogCategory& lc, LogLevel loglvl, const char* szFormat, ...)
    {
        constexpr size_t lc_len_max = LogCategory::MAX_LEN;
        constexpr size_t loglvl_len_max = 32ULL;
        constexpr size_t decor_len = 6ULL; // [,],\s,[,],\t
        constexpr size_t len_extra_max = lc_len_max + loglvl_len_max + decor_len;

        char buff[lc_len_max + loglvl_len_max + 1024];
        snprintf(buff, lc_len_max, "[%s] [%s]\t", (const char*) lc, LogLevelToString(loglvl));
        const size_t len_extra = strlen(buff);

        va_list arg;
        va_start(arg, szFormat);
        _vsnprintf(buff + len_extra, sizeof(buff) - len_extra, szFormat, arg);
        va_end(arg);

        OutputDebugString(buff);
        OutputDebugString("\n");
        if (loglvl >= LogLevel::Error && !nobreakonerror)
        {
            debug_break();
        }
    }
};
extern Logger g_Logger;

}