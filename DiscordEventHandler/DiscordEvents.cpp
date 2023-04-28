#include "DiscordEvents.hpp"

#include <iostream>
namespace discord {

nlohmann::json DiscordEvents::on_message_create(const nlohmann::json& data, ResponseCallback response_callback) {
    std::string content = data["content"];
    std::string channelId = data["channel_id"];
     nlohmann::json dataToReturn;
        /*Check if the message that is recived is correct with the [QUESTION] at the beggining*/
    if (content.find("[QUESTION]") != std::string::npos) {

        /*Waits until function returns response*/
        std::string response = response_callback(content);
        /*Check if the response is not empty*/
        if (!response.empty()) {
            
            nlohmann::json dataToReturn{
                    {"action","send_message"},
                    {"content", response},
                    {"channel_id", channelId},
                };
                return dataToReturn;
            }
        }   

    return nlohmann::json();
}

} // namespace discord
