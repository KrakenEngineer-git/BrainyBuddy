#ifndef BRAINY_BUDDY_HPP
#define BRAINY_BUDDY_HPP

#include <string>
#include <memory>
#include "OpenAIClient/OpenAIClient.hpp"
#include "utilities/utilities.hpp"

class BrainyBuddy
{
public:
    BrainyBuddy();
    void run();

private:
    std::string bot_token_;
    std::string openai_api_key_;
    std::unique_ptr<OpenAIClient> openai_client_;

    std::string get_openai_response(const std::string &input);
};

#endif // BRAINY_BUDDY_HPP