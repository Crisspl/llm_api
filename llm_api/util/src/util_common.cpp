#include <llm_api/util/util_common.h>

#if defined(LLM_API_UTIL_WITH_OPENAI)
#include "impl/openai.h"
#endif

namespace llm_api::util
{

static llm_api::unique_ptr<Interface> CreateInterface(LlmVendor vendor)
{
    switch (vendor)
    {
#if defined(LLM_API_UTIL_WITH_OPENAI)
    case LlmVendor::OPENAI:
        return llm_api::make_unique<InterfaceOpenAI>();
#endif
    default:
        return nullptr;
    }
    return nullptr;
}

static Interface* GetInterface_impl(LlmVendor vendor)
{
    static llm_api::unique_ptr<Interface> g_interfaces[(uint32) LlmVendor::COUNT] = {};

    const uint32_t index = (uint32) vendor;
    if (g_interfaces[index])
        return g_interfaces[index].get();

    g_interfaces[index] = CreateInterface(vendor);
    return g_interfaces[index].get();
}

Interface* GetInterface(LlmVendor vendor)
{
    return GetInterface_impl(vendor);
}

}
