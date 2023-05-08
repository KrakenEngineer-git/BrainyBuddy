#include "BrainyBuddy.hpp"
#include "DiscordClient/DiscordClient.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>

BrainyBuddy::BrainyBuddy()
{
    const char *token_env_var = std::getenv("DISCORD_BOT_TOKEN");
    const char *openai_api_key_env_var = std::getenv("OPENAI_API_KEY");

    if (token_env_var == nullptr)
    {
        throw std::runtime_error("Error: DISCORD_BOT_TOKEN environment variable is not set");
    }

    if (openai_api_key_env_var == nullptr)
    {
        throw std::runtime_error("Error: OPENAI_API_KEY environment variable is not set");
    }

    bot_token_ = std::string(token_env_var);
    openai_api_key_ = std::string(openai_api_key_env_var);

    openai_client_ = make_unique<OpenAIClient>(openai_api_key_);
}

void BrainyBuddy::run()
{
    discord::DiscordEvents::ResponseCallback response_callback = [this](const std::string &input) {
        return get_openai_response(input);
    };

    discord::DiscordClient client(bot_token_, response_callback);
    client.connect("wss://gateway.discord.gg/?v=10&encoding=json");

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string BrainyBuddy::get_openai_response(const std::string &input)
{
    return openai_client_->generate_response(input);
}

