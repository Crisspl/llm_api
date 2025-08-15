#include <llm_api/openai/openai_completion.h>

#include <cpr/cpr.h>

namespace llm_api::openai
{

namespace impl
{
    static string Completion(OpenAITextModel model, string&& system_prompt, string&& prompt, const apikey_t& apikey)
    {
        message_array msgs;
        msgs.reserve(2);
        msgs.push_back(Message::System(std::move(system_prompt)));
        msgs.push_back(Message::User(std::move(prompt)));
        Message ans;
        Result res = ChatML(model, std::move(msgs), ans);

        if (res == Result::Success)
            return ans.content;
        return "Error occured!";
    }
}

string Completion(OpenAITextModel model, string&& system_prompt, string&& prompt)
{
    return impl::Completion(model, std::move(system_prompt), std::move(prompt), GetCurrentApiKey());
}

string Completion(OpenAITextModel model, string&& prompt)
{
    return Completion(model, "You are helpful assistant.", std::move(prompt));
}

}