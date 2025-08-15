#include <llm_api/openai/openai_chat.h>

namespace llm_api::openai
{

string ChatLLMConnection::MakeSummaryPromptFragment(const string& summary)
{
    if (summary.empty())
        return {};

    string prompt_summary;
    prompt_summary += "\nSummary of past conversation:\n";
    prompt_summary += "<summary>\n";
    prompt_summary += summary;
    prompt_summary += "</summary>\n";
    return prompt_summary;
}

string ChatLLMConnection::Chat(string&& user_msg)
{
    string prompt_summary;
    if (!m_summary.empty())
    {
        prompt_summary = MakeSummaryPromptFragment(m_summary);
    }

    // compression if needed
    if (m_dialogue.size() == m_dialogue_len_kept)
    {
        string prompt_past_dialogue;
        prompt_past_dialogue += "Conversation exchange so far:\n";
        for (const DialogueFragment& d : m_dialogue)
        {
            prompt_past_dialogue += "<user>";
            prompt_past_dialogue += d.user_msg.content;
            prompt_past_dialogue += "</user>";
            prompt_past_dialogue += "<assistant>";
            prompt_past_dialogue += d.assistant_msg.content;
            prompt_past_dialogue += "</assistant>";
        }

        static const char* const summary_sys_prompt = "your only task is to summarize the conversation so far based on the summary provided and the last few exchanges of messages.First of all, consider the keywords and establish a leading topic of discussion.";
        string text_to_sum_up = prompt_summary;
        text_to_sum_up += prompt_past_dialogue;

        m_summary = Completion(m_model, summary_sys_prompt, std::move(text_to_sum_up));
        m_dialogue.clear();
    }

    if (m_dialogue.empty() && !m_summary.empty()) // just regenerated summary
    {
        prompt_summary = MakeSummaryPromptFragment(m_summary);
    }

    DialogueFragment new_dialogue_fragment;
    new_dialogue_fragment.user_msg = Message::User(std::move(user_msg));

    message_array msgs;
    msgs.reserve(1U + 1U + 2U * m_dialogue_len_kept + 1U);
    msgs.push_back(Message::System(string(m_role_definition)));
    if (!prompt_summary.empty())
    {
        msgs.push_back(Message::System(std::move(prompt_summary)));
    }
    for (const DialogueFragment& d : m_dialogue)
    {
        msgs.push_back(d.user_msg);
        msgs.push_back(d.assistant_msg);
    }
    msgs.push_back(new_dialogue_fragment.user_msg);

    ChatML(m_model, std::move(msgs), new_dialogue_fragment.assistant_msg);
    const string ans = new_dialogue_fragment.assistant_msg.content;
    m_dialogue.push_back(std::move(new_dialogue_fragment));

    return ans;
}

}