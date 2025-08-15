#include "llm_api_common_private.h"

#include <cpr/cpr.h>

#include <cstdio>

namespace llm_api
{
#if LLM_API_LOGGER_ENABLE
const bool Logger::nobreakonerror = true;

Logger g_Logger;
#endif

Result ChatML(apientrypoint_t apiep, apikey_t apikey, modelname_t model, message_array&& msgs, Message& output)
{
    output = Message::Assistant("");

    const ::std::string apikey_str = (::std::string) apikey; // TODO why i cant concatenate string and string_view??

    json requestBody = {
        {"model", model },
        {"messages", std::move(msgs)}
    };

    auto response = cpr::Post(
        cpr::Url{ apiep },
        cpr::Header{
            {"Authorization", ::std::string("Bearer ") + apikey_str},
            {"Content-Type", "application/json"}
        },
        cpr::Body{ requestBody.dump() }
    );

    if (response.status_code == 200) {
        auto jsonResponse = json::parse(response.text);
        string response = jsonResponse["choices"][0]["message"]["content"];
        output.content = std::move(response);
        return Result::Success;
    }
    else {
        output.content = "Error: HTTP " + llm_api::to_string(response.status_code);
        return Result::Failure;
    }
}

Result Completion_ImageResponse(apientrypoint_t apiep, apikey_t apikey, modelname_t model, string_view prompt, ImageExtent size, image_url_t& output)
{
    const ::std::string apikey_str = (::std::string)apikey; // TODO why i cant concatenate string and string_view??

    char size_str[50];
    std::sprintf(size_str, "%ux%u", (uint32) size.width, (uint32) size.height);

    json requestBody = {
        {"model", model },
        {"prompt", prompt },
        {"n", 1},
        {"size", size_str},
        {"response_format", "url"}
    };

    auto response = cpr::Post(
        cpr::Url{ apiep },
        cpr::Header{
            {"Authorization", ::std::string("Bearer ") + apikey_str},
            {"Content-Type", "application/json"}
        },
        cpr::Body{ requestBody.dump() }
    );

    string imageUrl;

    if (response.status_code == 200) {
        // Parse the response JSON
        json jsonResponse = json::parse(response.text);

        // Access the URL of the first generated image
        if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty()) 
        {
            imageUrl = jsonResponse["data"][0].value("url", "");
        }
        else 
        {
            imageUrl = "Error: 'data' field missing or empty in response.";
            return Result::Failure;
        }
    }
    else {
        imageUrl = "HTTP error: " + llm_api::to_string(response.status_code);
        return Result::Failure;
    }

    output = std::move(imageUrl);
    return Result::Success;
}

Result Completion_ImageResponse(apientrypoint_t ep, apikey_t apikey, modelname_t model, string_view prompt, ImageExtent size, Image& output)
{
    image_url_t url;
    Completion_ImageResponse(ep, apikey, model, prompt, size, url);

    {
        auto ImgNameFromUrl = [](const image_url_t& url) -> string {
            const size_t qmark = url.find_last_of('?');
            if (qmark == url.npos)
                return "";
            const size_t last_slash = url.find_last_of('/', qmark);
            if (last_slash == url.npos)
                return "";
            return url.substr(last_slash + 1U, qmark - last_slash - 1U);
        };

        output.name = ImgNameFromUrl(url);
        output.extent = size;
    }

    cpr::Response r = cpr::Get(cpr::Url{ (string_view) url });

    if (r.status_code == 200) 
    {
        const ::std::string& image_data = r.text;

        output.data = image_data_vector(image_data.begin(), image_data.end());

        return Result::Success;
    }

    return Result::Failure;
}

TokenCount Interface::ComputeTokenCount(llm_enum_underlying_t model, const Message& txt)
{
    bool is_input = false;
    if (txt.role == "system" || txt.role == "user")
        is_input = true;
#if LLM_API_DEBUG_LAYER
    else if (txt.role == "assistant")
        is_input = false;
    else { LLM_API_ASSERT(0); }
#endif

    const size_t tcount = ComputeTokenCount(model, txt.content);

    TokenCount tc;
    if (is_input)
    {
        tc.input = tcount;
        tc.output = 0ULL;
    }
    else
    {
        tc.input = 0ULL;
        tc.output = tcount;
    }

    return tc;
}

double Interface::MessageToUSDCost(llm_enum_underlying_t model, const Message& txt)
{
    return TokenCountToUSDCost(model, ComputeTokenCount(model, txt));
}

}