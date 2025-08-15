#pragma once

#include <llm_api/util/util_common.h>
#include <llm_api/openai/openai_common.h>
#include <llm_api/openai/openai_tiktoken.h>
#include <llm_api/openai/openai_cost.h>

namespace llm_api::util
{

class InterfaceOpenAI final : public Interface
{
public:
    bool IsInitialized() override
    {
        return openai::IsInitialized();
    }
    Result Initialize(apikey_t apikey) override
    {
        return openai::Initialize(apikey);
    }
    Result Finalize() override
    {
        return openai::Finalize();
    }

    bool IsFeatureSupported(FeatureFlags feature) override
    {
        return openai::IsFeatureSupported(feature);
    }

    modelname_t TextModelToString(llm_enum_underlying_t model) override
    {
        return openai::OpenAIModelToString((openai::OpenAITextModel) model);
    }
    llm_enum_underlying_t TextModelStringToEnum(modelname_t modelname) override
    {
        return (llm_enum_underlying_t) openai::OpenAIModelStringToEnum(modelname);
    }

    Result ChatML(llm_enum_underlying_t model, message_array&& msgs, Message& output) override
    {
        LLM_API_ASSERT(IsInitialized());
        return openai::ChatML((openai::OpenAITextModel) model, std::move(msgs), output);
    }

    size_t ComputeTokenCount(llm_enum_underlying_t model, const string& txt) override
    {
        return openai::ComputeTokenCount((openai::OpenAITextModel) model, txt);
    }

    double TokenCountToUSDCost(llm_enum_underlying_t model, const TokenCount& token_count) override
    {
        return openai::TokenCountToUSDCost((openai::OpenAITextModel) model, token_count);
    }
};

}