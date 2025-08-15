#pragma once

#include <llm_api/openai/openai_common.h>

namespace llm_api::openai
{

size_t ComputeTokenCount(OpenAITextModel model, const llm_api::string& txt);

}