#include "DiscordEvents.hpp"

#include <iostream>
#include <regex>
#include <vector>
namespace discord {

nlohmann::json DiscordEvents::on_message_create(const nlohmann::json& data,CheckIfIsAQuestion check_if_question, ResponseCallback response_callback) {
    std::string content = data.value("content", "NOT FOUND");
    std::string channelId = data.value("channel_id", "NOT FOUND");
    std::string messageId = data.value("id", "NOT FOUND");

    std::string username = "NOT_FOUND";
    bool isBot = false;

    if (data.contains("author")) {
        username = data["author"].value("username", "NOT_FOUND");
        isBot = data["author"].value("bot", false);
    }

    nlohmann::json dataToReturn;
    
    if (!isBot) 
    {
        std::cout << "User: " << username << "Send: " <<  content << std::endl;

        std::string response = "";

        check_if_question(content) ?  response = response_callback(content,username) : response;
        
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
        else
        {
            return nlohmann::json();
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
