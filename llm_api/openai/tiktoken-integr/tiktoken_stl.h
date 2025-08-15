#pragma once

#include <llm_api/common/llm_api_common.h>

#define TIKTOKEN_STL_TYPEDEFS_DEFINED 1
namespace tiktoken::tt_stl
{
    using llm_api::getline;
    using llm_api::to_string;

    using llm_api::string;
    using llm_api::vector;
    using llm_api::unordered_set;
    using llm_api::unordered_map;
}