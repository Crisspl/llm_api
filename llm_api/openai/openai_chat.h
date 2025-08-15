#include "openai_completion.h"

#include <list> 

namespace llm_api
{

class InterfaceChatLLM
{
public:
    virtual ~InterfaceChatLLM() = default;

    virtual string Chat(string&& user_msg) = 0;
};

}

namespace llm_api::openai
{

class ChatLLMConnection : public llm_api::InterfaceChatLLM
{
    OpenAITextModel m_model;
    string m_role_definition;
    string m_summary;
    std::list<DialogueFragment> m_dialogue;
    const size_t m_dialogue_len_kept;

public:
    ChatLLMConnection(OpenAITextModel model, string&& role_def, size_t max_len)
        : m_model(model)
        , m_role_definition(std::move(role_def))
        , m_dialogue_len_kept(max_len)
    {
    }

    string Chat(string&& user_msg) override;

private:
    static string MakeSummaryPromptFragment(const string& summary);
};

}