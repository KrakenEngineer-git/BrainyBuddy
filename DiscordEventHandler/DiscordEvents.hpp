#ifndef DISCORD_EVENTS_HPP
#define DISCORD_EVENTS_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace discord {

class DiscordEvents {
public:
    using CheckIfIsAQuestion = std::function<bool(const std::string&)>;
    using ResponseCallback = std::function<std::string(const std::string&, const std::string&)>;

    nlohmann::json on_message_create(const nlohmann::json& data, CheckIfIsAQuestion check_if_question, ResponseCallback response_callback);
    nlohmann::json on_message_update(const nlohmann::json& data);
    nlohmann::json on_message_delete(const nlohmann::json& data);
};

} // namespace discord

#endif // DISCORD_EVENTS_HPP
