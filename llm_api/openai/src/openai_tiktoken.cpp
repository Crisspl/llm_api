#include <llm_api/openai/openai_tiktoken.h>

#include <mutex>
#include <array>

// tiktoken
#include "../tiktoken-integr/tiktoken_stl.h"
#include <encoding.h>
#include <embedded_resource_reader.h>

namespace llm_api::openai
{
struct Tokenizer
{
    Tokenizer(tiktoken::GptEncoding&& t) : tokenizer(std::move(t)) {}
    ~Tokenizer()
    {
        volatile std::unique_lock<std::mutex> lk(mut);
        // make sure to own the lock before destruction
    }

    tiktoken::GptEncoding tokenizer;
    std::mutex mut;
};

static std::array<unique_ptr<Tokenizer>, (size_t) tiktoken::LanguageModel::COUNT> g_tokenizers;

Result TiktokenInitialize()
{
    for (size_t tk = 0ULL; tk < g_tokenizers.size(); ++tk)
    {
        g_tokenizers[tk] = make_unique<Tokenizer>(tiktoken::GptEncoding::get_encoding((tiktoken::LanguageModel) tk));
    }

    return Result::Success;
}

Result TiktokenFinalize()
{
    for (size_t tk = 0ULL; tk < g_tokenizers.size(); ++tk)
    {
        g_tokenizers[tk] = nullptr;
    }

    return Result::Success;
}

static tiktoken::LanguageModel OpenAIModelToTokenizer(OpenAITextModel model)
{
    using namespace tiktoken;

    static constexpr LanguageModel model2tokenizer[(size_t) OpenAITextModel::COUNT] = {
        LanguageModel::O200K_BASE,// GPT_4O,
        LanguageModel::O200K_BASE,// GPT_4O_MINI,
        LanguageModel::CL100K_BASE,// GPT_4_1,
        LanguageModel::CL100K_BASE,// GPT_4_1_MINI,
        LanguageModel::CL100K_BASE,// GPT_4_1_NANO,
        LanguageModel::CL100K_BASE,// GPT_3_5_TURBO,
        LanguageModel::O200K_BASE,// GPT_5,
        LanguageModel::O200K_BASE,// GPT_5_MINI,
        LanguageModel::O200K_BASE,// GPT_5_NANO,
        LanguageModel::O200K_BASE,// O3,
        LanguageModel::O200K_BASE,// O3_MINI,
        LanguageModel::O200K_BASE,// O4_MINI,
    };

    return model2tokenizer[(uint32) model];
}

size_t ComputeTokenCount(OpenAITextModel model, const llm_api::string& txt)
{
    auto GetTokenizer = [] (OpenAITextModel model) -> Tokenizer* {
        const tiktoken::LanguageModel tk = OpenAIModelToTokenizer(model);
        if (Tokenizer* tokenizer = g_tokenizers[(size_t) tk].get())
        {
            LLM_API_ASSERT(tokenizer);
            return tokenizer;
        }

        LLM_API_ASSERT(false);
        return nullptr;
    };

    Tokenizer* const tokenizer = GetTokenizer(model);
    std::unique_lock<std::mutex> lk(tokenizer->mut);

    return tokenizer->tokenizer.encode(txt).size();
}

}