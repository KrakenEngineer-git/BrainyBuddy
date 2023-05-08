#include "DiscordEvents.hpp"

#include <iostream>
namespace discord {

nlohmann::json DiscordEvents::on_message_create(const nlohmann::json& data, ResponseCallback response_callback) {
    std::string content = data["content"];
    std::string channelId = data["channel_id"];
    std::string messageId = data["id"];
    std::string username = data["author"]["username"];
    bool isBot = false; 

    if (data["author"].contains("bot")) {
        isBot = data["author"]["bot"].get<bool>();  // Extract bot status from the message
    }

    nlohmann::json dataToReturn;

    /*Check if the message that is received is prefixed with "!question" at the beginning and the sender is not a bot*/
    if (content.rfind("!question ", 0) == 0 && !isBot) {

        content = content.substr(10); // Remove "!question" from the message

        std::cout << username << " sent: " << content << std::endl;

        /*Waits until function returns response*/
        std::string response = response_callback(content);

        /*Check if the response is not empty*/
        if (!response.empty()) {
            dataToReturn = {
                {"action","send_message"},
                {"username", username},
                {"content", response},
                {"channel_id", channelId},
                {"message_id", messageId},
            };
            return dataToReturn;
        }
    }   

    return nlohmann::json();
}



nlohmann::json DiscordEvents::on_message_update(const nlohmann::json& data) {
    // Handle MESSAGE_UPDATE event
    std::cout << "Message updated " << std::endl;
    return data;
}

nlohmann::json DiscordEvents::on_message_delete(const nlohmann::json& data) {
    // Handle MESSAGE_DELETE event
    std::cout << "Message deleted " << std::endl;
    return data;
}


} // namespace discord
