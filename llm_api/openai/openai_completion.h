#pragma once

#include <string>
#include <string_view>

#include "openai_common.h"

namespace llm_api::openai
{

// system prompt is "You are helpful assistant."
string Completion(OpenAITextModel model, string&& prompt);

string Completion(OpenAITextModel model, string&& system_prompt, string&& prompt);

}