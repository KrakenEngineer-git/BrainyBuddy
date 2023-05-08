#include "OpenAIClient.hpp"
#include <iostream>
OpenAIClient::OpenAIClient(const std::string &api_key) : api_key_(api_key)
{
    curl_handler_ = make_unique<CurlHandler>();
    curl_handler_->AddHeader("Authorization: Bearer " + api_key_);
    curl_handler_->AddHeader("Content-Type: application/json");
}

std::string OpenAIClient::generate_response(const std::string &input, const std::string &author_username)
{
    std::string url = "https://api.openai.com/v1/chat/completions";

    nlohmann::json payload = {
        {"model", "gpt-3.5-turbo"},
        {
            "messages",
            {
                {
                    {"role", "system"},
                    {"content", "You are an AI assistant, and you are now talking to " + author_username + "."}
                },
                {
                    {"role", "user"},
                    {"content", input}
                }
            }
        },
        {"max_tokens", 50},
        {"top_p", 1},
        {"stop", nullptr},
        {"temperature", 0.5},
    };

    std::string data = payload.dump();

    std::string response = curl_handler_->post(url, data, true);

    nlohmann::json response_json = nlohmann::json::parse(response);
    std::string generated_text = response_json["choices"][0]["message"]["content"];

    return generated_text;
}
