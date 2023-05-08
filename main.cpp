#include "DiscordClient/DiscordClient.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main() {
    const char* token_env_var = std::getenv("DISCORD_BOT_TOKEN");

    if (token_env_var == nullptr) {
        std::cerr << "Error: DISCORD_BOT_TOKEN environment variable is not set" << std::endl;
        return 0;
    }
    std::string token(token_env_var);

    discord::DiscordEvents::ResponseCallback response_callback = [](const std::string& content) {
        return std::string("Answer to: ") + content;
    };

    discord::DiscordClient client(token, response_callback);
    client.connect("wss://gateway.discord.gg/?v=9&encoding=json");

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
