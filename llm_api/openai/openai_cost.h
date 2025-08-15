#pragma once

#include <llm_api/openai/openai_tiktoken.h>

namespace llm_api::openai
{

double TokenCountToUSDCost(OpenAITextModel model, const TokenCount& token_count);
double TokenCountToUSDCost(const TokenCountPerModel& token_count);

}