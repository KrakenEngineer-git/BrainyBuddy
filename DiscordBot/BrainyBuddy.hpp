#ifndef BRAINY_BUDDY_HPP
#define BRAINY_BUDDY_HPP

#include <string>
#include <memory>
#include <atomic>
#include "OpenAIClient/OpenAIClient.hpp"
#include "DiscordClient/DiscordClient.hpp"
#include "utilities/utilities.hpp"

class BrainyBuddy
{
public:
    BrainyBuddy();
    ~BrainyBuddy();
    void run();
    
    void cleanup();
    static BrainyBuddy* getInstance(); 

private:
    static void signalHandler(int signal);
    std::string bot_token_;
    std::string openai_api_key_;
    std::unique_ptr<discord::DiscordClient> discord_client_;
    std::unique_ptr<OpenAIClient> openai_client_;

    bool check_if_question(const std::string &input);
    std::string get_openai_response(const std::string &input,const std::string &author_username);

    static BrainyBuddy* instance_;
    static std::atomic<bool> quit_;
};

#endif // BRAINY_BUDDY_HPP
