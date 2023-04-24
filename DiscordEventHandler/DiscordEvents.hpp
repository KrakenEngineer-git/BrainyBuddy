#ifndef DISCORD_EVENTS_HPP
#define DISCORD_EVENTS_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace discord {

class DiscordEvents {
public:
    using ResponseCallback = std::function<std::string(const std::string&)>;

    nlohmann::json on_message_create(const nlohmann::json& data, ResponseCallback response_callback);
};

} // namespace discord

#endif // DISCORD_EVENTS_HPP
