# `llm_api`
C++ library aiming to unify and abstract communication with various LLMs of multiple LLM vendors, such as OpenAI.

## Currently suported LLM vendors
* OpenAI

More will be coming step by step. You're welcome to contribute a LLM vendor backend that is currently missing via PR.

## How to use

In general, `llm_api` can be used in three ways (on example of OpenAI):

### Direct vendor API
```
#include <llm_api/openai/openai_common.h>

llm_api::openai::Initialize("<your-api-key>");

llm_api::message_array input = {
  llm_api::Message::System("You are helpful assistant."),
  llm_api::Message::User("Please help me with this thing!")
};
llm_api::Message output;

const llm_api::Result result = llm_api::openai::ChatML(
        llm_api::openai::OpenAITextModel::GPT_5_NANO,
        std::move(input),
        output);
assert(result == llm_api::Result::Success);

printf("%s\n", output.content.c_str());

llm_api::openai::Finalize();
```

### Using `llm_api::util` via interface
```
#include <llm_api/openai/openai_common.h>
#include <llm_api/util/util_common.h>

llm_api::Interface* const openai = llm_api::util::GetInterface(llm_api::LlmVendor::OPENAI, "<your-api-key>");
// Or:
// llm_api::Interface* const openai = llm_api::util::GetInterface(llm_api::LlmVendor::OPENAI);
// openai->Initialize("<your-api-key>");

llm_api::message_array input = {
  llm_api::Message::System("You are helpful assistant."),
  llm_api::Message::User("Please help me with this thing!")
};
llm_api::Message output;

const llm_api::Result result =
openai->ChatML((llm_api::llm_enum_underlying_t) llm_api::openai::OpenAITextModel::GPT_5_NANO,
        std::move(input),
        output);
assert(result == llm_api::Result::Success);

printf("%s\n", output.content.c_str());

openai->Finalize();
```

### Using `llm_api::util` generic API
```
#include <llm_api/openai/openai_common.h>
#include <llm_api/util/util_common.h>

llm_api::util::Initialize(llm_api::LlmVendor::OPENAI, "<your-api-key>");

llm_api::message_array input = {
  llm_api::Message::System("You are helpful assistant."),
  llm_api::Message::User("Please help me with this thing!")
};
llm_api::Message output;

const llm_api::LlmTextModel model = {
        llm_api::LlmVendor::OPENAI,
        (llm_api::llm_enum_underlying_t) llm_api::openai::OpenAITextModel::GPT_5_NANO,
};

const llm_api::Result result = llm_api::util::ChatML(model, std::move(input), output);
assert(result == llm_api::Result::Success);

printf("%s\n", output.content.c_str());

llm_api::util::Finalize(llm_api::LlmVendor::OPENAI);
```

## Extra features
Each LLM vendor backend can be queried for supported extra features using `IsFeatureSupported` functions.
* Computation of token count (`ComputeTokenCount()`)
* Approximation of cost in US dollars (`TokenCountToUSDCost()`, `MessageToUSDCost()`)

See `llm_api::FeatureFlags` for list of potentially available features.
