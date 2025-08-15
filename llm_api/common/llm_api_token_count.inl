#pragma once

struct TokenCount
{
    size_t input = 0ULL;
    size_t output = 0ULL;

    TokenCount& operator+=(const TokenCount& tc)
    {
        input += tc.input;
        output += tc.output;
        return *this;
    }
    TokenCount operator+(const TokenCount& tc) const
    {
        TokenCount retval{ input, output };
        return retval += tc;
    }
    void Reset()
    {
        input = 0ULL;
        output = 0ULL;
    }
};

template <LlmVendor Vendor>
struct TokenCountPerModel
{
    using model_enum_t = llm_text_model_enum_for_vendor_t<Vendor>;
    enum : llm_enum_underlying_t { MODEL_COUNT = llm_text_model_enum_for_vendor_t<Vendor>::COUNT };
    TokenCount token_count[(size_t) MODEL_COUNT];

    void Add(typename model_enum_t model, const TokenCount& tc)
    {
        token_count[(uint32) model] += tc;
    }
    void Add(const TokenCountPerModel& tc)
    {
        for (uint32 i = 0U; i < MODEL_COUNT; ++i)
            Add((model_enum_t) i, tc.token_count[i]);
    }
    TokenCountPerModel& operator+=(const TokenCountPerModel& tc)
    {
        Add(tc);
        return *this;
    }
    void Reset()
    {
        for (uint32 i = 0U; i < MODEL_COUNT; ++i)
            token_count[i].Reset();
    }
};
