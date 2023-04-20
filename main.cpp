#include "WebSocketHandler/impl/WebsocketClient.hpp"
#include "WebSocketHandler/impl/WebsocketClientHandler.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main() 
{
    const char* token_env_var = std::getenv("DISCORD_BOT_TOKEN");

    if (token_env_var == nullptr) {
        std::cerr << "Error: DISCORD_BOT_TOKEN environment variable is not set" << std::endl;
        return 0 ;
    }
    std::string token(token_env_var);
    websocket_handler::WebsocketClientHandler handler(token);
    handler.connect("wss://gateway.discord.gg/?v=9&encoding=json");
    handler.send("Hello World!");

    return 0;
}