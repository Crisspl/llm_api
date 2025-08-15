#include <llm_api/openai/openai_common.h>
#include <llm_api/common/src/llm_api_common_private.h>

namespace llm_api::openai
{

static apikey_t g_apikey;
static const apientrypoint_t g_apientrypoint_chat = "https://api.openai.com/v1/chat/completions";
static const apientrypoint_t g_apientrypoint_imggen = "https://api.openai.com/v1/images/generations";

static const FeatureFlags g_SupportedFeatures = 
    FeatureFlags::TOKEN_COUNT |
    FeatureFlags::COST_APPROX;

Result Initialize(apikey_t apikey)
{
    extern Result TiktokenInitialize();

    if (IsInitialized())
    {
        LLM_API_WARNING(LC_OpenAI, "lm_api::openai already initialized!");
        return Result::Failure;
    }
    if (!IsApiKey(apikey))
    {
        LLM_API_ERROR(LC_OpenAI, "Failed to initialize llm_api::openai!");
        return Result::Failure;
    }
    if (TiktokenInitialize() != Result::Success)
    {
        LLM_API_ERROR(LC_OpenAI, "Failed to initialize llm_api::openai! (tiktoken module initialization failed)");
        return Result::Failure;
    }

    g_apikey = apikey;

    LLM_API_INFO(LC_OpenAI, "Successfully initialized llm_api::openai!");

    return Result::Success;
}

Result Finalize()
{
    extern Result TiktokenFinalize();

    if (TiktokenFinalize() != Result::Success)
    {
        LLM_API_ERROR(LC_OpenAI, "Failed to finalize llm_api::openai! (tiktoken module finalization failed)");

        return Result::Failure;
    }

    LLM_API_INFO(LC_OpenAI, "Successfully finalized llm_api::openai!");

    return Result::Success;
}

bool IsInitialized()
{
    return !g_apikey.empty();
}

bool IsFeatureSupported(FeatureFlags feature)
{
    return (g_SupportedFeatures & feature) != (FeatureFlags) 0;
}

apikey_t GetCurrentApiKey()
{
    return g_apikey;
}

apientrypoint_t GetApiEntryPoint(EntryPointType ept)
{
    switch (ept)
    {
    case EntryPointType::ChatCompletion:
        return g_apientrypoint_chat;
    case EntryPointType::ImageGeneration:
        return g_apientrypoint_imggen;
    default:
        return "";
    }
}

Result ChatML(OpenAITextModel model, message_array&& msgs, Message& output)
{
    return llm_api::ChatML(GetApiEntryPoint(EntryPointType::ChatCompletion), GetCurrentApiKey(), OpenAIModelToString(model), std::move(msgs), output);
}

Result Completion_ImageResponse(OpenAIImageModel model, string_view prompt, ImageExtent size, image_url_t& output)
{
    return llm_api::Completion_ImageResponse(GetApiEntryPoint(EntryPointType::ImageGeneration), GetCurrentApiKey(), OpenAIModelToString(model), prompt, size, output);
}

Result Completion_ImageResponse(OpenAIImageModel model, string_view prompt, ImageExtent size, Image& output)
{
    return llm_api::Completion_ImageResponse(GetApiEntryPoint(EntryPointType::ImageGeneration), GetCurrentApiKey(), OpenAIModelToString(model), prompt, size, output);
}



}