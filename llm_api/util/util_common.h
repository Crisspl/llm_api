#pragma once

#include <llm_api/common/llm_api_common.h>
#include <limits>

namespace llm_api::util
{

Interface* GetInterface(LlmVendor vendor);
// Tries to initialize and returns nullptr on failure
inline Interface* GetInterface(LlmVendor vendor, apikey_t apikey)
{
    Interface* iface = GetInterface(vendor);
    if (iface && !iface->IsInitialized() && iface->Initialize(apikey) != Result::Success)
        return nullptr;

    return iface;
}

template <LlmVendor Vendor>
inline double TokenCountToUSDCost(const TokenCountPerModel<Vendor>& token_count)
{
    Interface* const iface = GetInterface(Vendor);
    if (!iface)
        return -1.0;

    double cost = 0.0;
    for (llm_enum_underlying_t i = 0U; i < (llm_enum_underlying_t) TokenCountPerModel<Vendor>::MODEL_COUNT; ++i)
        cost += iface->TokenCountToUSDCost(i, token_count.token_count[i]);

    return cost;
}

inline bool IsInitialized(LlmVendor vendor)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->IsInitialized();
    return false;
}
inline Result Initialize(LlmVendor vendor, apikey_t apikey)
{
    if (GetInterface(vendor, apikey) != nullptr)
        return Result::Success;
    return Result::Failure;
}
inline Result Finalize(LlmVendor vendor)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->Finalize();
    return Result::Failure;
}

inline bool IsFeatureSupported(LlmVendor vendor, FeatureFlags feature)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->IsFeatureSupported(feature);
    return false;
}

inline modelname_t TextModelToString(LlmVendor vendor, llm_enum_underlying_t model)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->TextModelToString(model);
    return modelname_t{};
}
inline modelname_t TextModelToString(LlmTextModel model)
{
    return TextModelToString(model.vendor, model.llm);
}

inline llm_enum_underlying_t TextModelStringToEnum(LlmVendor vendor, modelname_t modelname)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->TextModelStringToEnum(modelname);
    return std::numeric_limits<llm_enum_underlying_t>::max();
}

inline Result ChatML(LlmVendor vendor, llm_enum_underlying_t model, message_array&& msgs, Message& output)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->ChatML(model, std::move(msgs), output);
    return Result::Failure;
}
inline Result ChatML(LlmTextModel model, message_array&& msgs, Message& output)
{
    return ChatML(model.vendor, model.llm, std::move(msgs), output);
}

inline size_t ComputeTokenCount(LlmVendor vendor, llm_enum_underlying_t model, const string& txt)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->ComputeTokenCount(model, txt);
    return (size_t) 0;
}
inline size_t ComputeTokenCount(LlmTextModel model, const string& txt)
{
    return ComputeTokenCount(model.vendor, model.llm, txt);
}
inline TokenCount ComputeTokenCount(LlmVendor vendor, llm_enum_underlying_t model, const Message& txt)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->ComputeTokenCount(model, txt);
    return TokenCount{};
}
inline TokenCount ComputeTokenCount(LlmTextModel model, const Message& txt)
{
    return ComputeTokenCount(model.vendor, model.llm, txt);
}

inline double TokenCountToUSDCost(LlmVendor vendor, llm_enum_underlying_t model, const TokenCount& token_count)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->TokenCountToUSDCost(model, token_count);
    return -1.0;
}
inline double TokenCountToUSDCost(LlmTextModel model, const TokenCount& token_count)
{
    return TokenCountToUSDCost(model.vendor, model.llm, token_count);
}
inline double MessageToUSDCost(LlmVendor vendor, llm_enum_underlying_t model, const Message& txt)
{
    if (Interface* iface = GetInterface(vendor))
        return iface->MessageToUSDCost(model, txt);
    return -1.0;
}
inline double MessageToUSDCost(LlmTextModel model, const Message& txt)
{
    return MessageToUSDCost(model.vendor, model.llm, txt);
}

}