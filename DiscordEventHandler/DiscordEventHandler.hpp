#ifndef DISCORD_EVENT_HANDLER_HPP
#define DISCORD_EVENT_HANDLER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include "nlohmann/json.hpp"

namespace discord {

class DiscordEventHandler {
public:
    using event_handler_t = std::function<void(const nlohmann::json&)>;

    void register_event_handler(const std::string& event_name, event_handler_t handler);
    void handle_event(const std::string& payload, std::function<void(const std::string&)> error_handler);

private:
    std::unordered_map<std::string, event_handler_t> event_handlers_;
};

} // namespace discord

#endif // DISCORD_EVENT_HANDLER_HPP