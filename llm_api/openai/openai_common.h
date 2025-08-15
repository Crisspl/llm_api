#pragma once

#include <llm_api/common/llm_api_common.h>
#include <string_view>
#include <utility>
#include <array>
#include <iterator>

namespace llm_api::openai
{

constexpr static inline LogCategory LC_OpenAI("OpenAI");

enum : uint32
{
    APIKEY_MAX_LEN = 256U,
};


enum class OpenAITextModel : llm_enum_underlying_t
{
    GPT_4O,
    GPT_4O_MINI,
    GPT_4_1,
    GPT_4_1_MINI,
    GPT_4_1_NANO,
    GPT_3_5_TURBO,
    GPT_5,
    GPT_5_MINI,
    GPT_5_NANO,
    O3,
    O3_MINI,
    O4_MINI,

    COUNT
};

inline constexpr modelname_t OpenAIModelToString(OpenAITextModel model) 
{
    switch (model) {
    case OpenAITextModel::GPT_4O:
        return "gpt-4o";
    case OpenAITextModel::GPT_4O_MINI:
        return "gpt-4o-mini";
    case OpenAITextModel::GPT_4_1:
        return "gpt-4.1";
    case OpenAITextModel::GPT_4_1_MINI:
        return "gpt-4.1-mini";
    case OpenAITextModel::GPT_4_1_NANO:
        return "gpt-4.1-nano";
    case OpenAITextModel::GPT_3_5_TURBO:
        return "gpt-3.5-turbo";
    case OpenAITextModel::GPT_5:
        return "gpt-5";
    case OpenAITextModel::GPT_5_MINI:
        return "gpt-5-mini";
    case OpenAITextModel::GPT_5_NANO:
        return "gpt-5-nano";
    case OpenAITextModel::O3:
        return "o3";
    case OpenAITextModel::O3_MINI:
        return "o3-mini";
    case OpenAITextModel::O4_MINI:
        return "o4-mini";
    default:
        return "";  // Unknown model
    }
}

inline constexpr OpenAITextModel OpenAIModelStringToEnum(modelname_t model)
{
    constexpr auto MODEL_NAMES = [] () constexpr {
        std::array<modelname_t, (size_t) OpenAITextModel::COUNT> models;

        for (uint32 i = 0U; i < (uint32) OpenAITextModel::COUNT; ++i)
        {
            models[i] = openai::OpenAIModelToString((OpenAITextModel) i);
        }

        return models;
    }();

    auto found = std::find(std::cbegin(MODEL_NAMES), std::cend(MODEL_NAMES), model);
    if (found == std::end(MODEL_NAMES))
        return OpenAITextModel::COUNT;

    return (OpenAITextModel) std::distance(std::cbegin(MODEL_NAMES), found);
}

enum class OpenAIImageModel : llm_enum_underlying_t
{
    GPT_IMAGE_1,
    DALL_E_2,
    DALL_E_3,

    COUNT
};

inline constexpr modelname_t OpenAIModelToString(OpenAIImageModel model) {
    switch (model) {
    case OpenAIImageModel::GPT_IMAGE_1:
        return "gpt-image-1";
    case OpenAIImageModel::DALL_E_2:
        return "dall-e-2";
    case OpenAIImageModel::DALL_E_3:
        return "dall-e-3";
    default:
        return ""; // Unknown
    }
}

inline bool IsApiKey(apikey_t apikey)
{
    static const char prefix[] = "sk-";
    return apikey.substr(0, sizeof(prefix)-1) == string_view(prefix);
}

Result Initialize(apikey_t apikey);
Result Finalize();
bool IsInitialized();

bool IsFeatureSupported(FeatureFlags feature);

apikey_t GetCurrentApiKey();
apientrypoint_t GetApiEntryPoint(EntryPointType ept);

Result ChatML(OpenAITextModel model, message_array&& msgs, Message& output);

Result Completion_ImageResponse(OpenAIImageModel model, string_view prompt, ImageExtent size, image_url_t& output);
Result Completion_ImageResponse(OpenAIImageModel model, string_view prompt, ImageExtent size, Image& output);

}

namespace llm_api
{
template <>
struct llm_text_model_enum_for_vendor<LlmVendor::OPENAI> { using model_enum = openai::OpenAITextModel; };

template <>
struct llm_image_model_enum_for_vendor<LlmVendor::OPENAI> { using model_enum = openai::OpenAIImageModel; };
}

namespace llm_api::openai
{
using TokenCountPerModel = ::llm_api::TokenCountPerModel<LlmVendor::OPENAI>;
}