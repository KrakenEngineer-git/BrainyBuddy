#include "DiscordEvents.hpp"

#include <iostream>
namespace discord {

nlohmann::json DiscordEvents::on_message_create(const nlohmann::json& data, ResponseCallback response_callback) {
    std::string content = data["content"];
    std::string channelId = data["channel_id"];
    std::string username = data["author"]["username"];
    nlohmann::json dataToReturn;
        /*Check if the message that is recived is correct with the [QUESTION] at the beggining*/
    if (content.find("[QUESTION]") != std::string::npos) {

        /*Waits until function returns response*/
        std::string response = response_callback(content);
        /*Check if the response is not empty*/
        if (!response.empty()) {
            
            nlohmann::json dataToReturn{
                    {"action","send_message"},
                    {"username", username},
                    {"content", response},
                    {"channel_id", channelId},
                };
                return dataToReturn;
            }
        }   

    return nlohmann::json();
}


nlohmann::json DiscordEvents::on_message_update(const nlohmann::json& data) {
    // Handle MESSAGE_UPDATE event
    std::cout << "Message updated: " << std::endl;
    return data;
}

nlohmann::json DiscordEvents::on_message_delete(const nlohmann::json& data) {
    // Handle MESSAGE_DELETE event
    std::cout << "Message deleted: " << std::endl;
    return data;
}


} // namespace discord
