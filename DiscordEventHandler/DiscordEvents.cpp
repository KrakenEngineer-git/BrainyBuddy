#include "DiscordEvents.hpp"

namespace discord {

nlohmann::json DiscordEvents::on_message_create(const nlohmann::json& data, ResponseCallback response_callback) {
    std::string content = data["content"];
    std::string channelId = data["channel_id"];
    
        /*Check if the message that is recived is correct with the [QUESTION] at the beggining*/
    if (content.find("[QUESTION]") != std::string::npos) {

        /*Waits until function returns response*/
        std::string response = response_callback(content);

        nlohmann::json{
                {"action","send_message"},
                {"content", response},
                {"channel_id", channelId},
            };
        }

    return nlohmann::json();
}

} // namespace discord