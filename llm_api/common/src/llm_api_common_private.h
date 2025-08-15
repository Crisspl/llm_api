#pragma once

#include <llm_api/common/llm_api_common.h>

#include <nlohmann/json.hpp>

namespace llm_api
{

using json = nlohmann::json;

inline void to_json(json& j, const Message& msg) {
    j = json{
        {"role", msg.role},
        {"content", msg.content}
    };
}

inline void from_json(const json& j, Message& msg) {
    string role;
    j.at("role").get_to(role);
#define IF_ROLE(r, m) if (role == #r) m.role = #r
    IF_ROLE(system, msg);
else IF_ROLE(user, msg);
else IF_ROLE(assistant, msg);
#undef IF_ROLE
    j.at("content").get_to(msg.content);
}

}