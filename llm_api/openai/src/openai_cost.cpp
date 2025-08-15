#include <llm_api/openai/openai_cost.h>
#include <llm_api/openai/openai_tiktoken.h>

namespace llm_api::openai
{

namespace
{
    // Cost per 1M tokens
    struct TokenCost
    {
        double input = 0.0;
        double output = 0.0;
    };

    TokenCost OpenAIModelToUSDCost(OpenAITextModel model)
    {
        static constexpr TokenCost model2cost[(size_t) OpenAITextModel::COUNT] = {
            { 2.5, 10.0 },// GPT_4O,
            { 0.15, 0.6 },// GPT_4O_MINI,
            { 2.0, 8.0 },// GPT_4_1,
            { 0.4, 1.6 },// GPT_4_1_MINI,
            { 0.1, 0.4 },// GPT_4_1_NANO,
            { 0.5, 1.5 },// GPT_3_5_TURBO,
            { 1.25, 10.0 }, // GPT_5,
            { 0.25, 2.0 }, // GPT_5_MINI,
            { 0.05, 0.4 }, // GPT_5_NANO,
            { 2.0, 8.0 },// O3,
            { 1.1, 4.4 },// O3_MINI,
            { 1.1, 4.4 },// O4_MINI,
        };

        return model2cost[(uint32) model];
    }
}

double TokenCountToUSDCost(OpenAITextModel model, const TokenCount& token_count)
{
    const TokenCost cost_per_1Mtoken = OpenAIModelToUSDCost(model);
    
    return (cost_per_1Mtoken.input * 1e-6 * token_count.input) + (cost_per_1Mtoken.output * 1e-6 * token_count.output);
}

double TokenCountToUSDCost(const TokenCountPerModel& token_count)
{
    double cost = 0.0;
    for (uint32 i = 0U; i < (uint32) OpenAITextModel::COUNT; ++i)
        cost += TokenCountToUSDCost((OpenAITextModel) i, token_count.token_count[i]);

    return cost;
}

}
