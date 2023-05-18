#include "BrainyBuddy.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <boost/asio.hpp>

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

    try {
        openai_client_ = make_unique<OpenAIClient>(openai_api_key_);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

BrainyBuddy::~BrainyBuddy()
{
    discord_client_->stop();
    std::cout << "BrainyBuddy destructor called" << std::endl;
}

void BrainyBuddy::run()
{
    discord::DiscordEvents::ResponseCallback response_callback = [this](const std::string &input, const std::string &author_username) {
        return get_openai_response(input,author_username);
    };

    discord::DiscordEvents::CheckIfIsAQuestion check_if_question = [this](const std::string &input) {
        return this->check_if_question(input);
    };

    try {
        discord_client_ = make_unique<discord::DiscordClient>(bot_token_,check_if_question,response_callback);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    
    discord_client_->run();
}

bool BrainyBuddy::check_if_question(const std::string &input)
{
    return openai_client_->is_question(input);
}

std::string BrainyBuddy::get_openai_response(const std::string &input,const std::string &author_username)
{
    return openai_client_->generate_response(input,author_username);
}

