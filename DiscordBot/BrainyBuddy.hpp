#ifndef BRAINY_BUDDY_HPP
#define BRAINY_BUDDY_HPP

#include "DiscordClient/DiscordClient.hpp"
#include "OpenAIClient/OpenAIClient.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include <atomic>
#include <memory>
#include <string>

class BrainyBuddy
{
  public:
    BrainyBuddy();
    ~BrainyBuddy();
    void run();

    void cleanup();
    static BrainyBuddy *getInstance();

  private:
    std::string bot_token_;
    std::string openai_api_key_;
    std::unique_ptr<discord::DiscordClient> discord_client_;
    std::unique_ptr<OpenAIClient> openai_client_;

    bool check_if_question(const std::string &input);
    std::string get_openai_response(const std::string &input, const std::string &author_username);

    std::mutex mtx_;
    ThreadPool threadPool;
    static std::atomic<bool> quit_;
};

#endif // BRAINY_BUDDY_HPP
