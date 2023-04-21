#include "DiscordEventHandler.hpp"

namespace discord {

void DiscordEventHandler::register_event_handler(const std::string& event_name, event_handler_t handler) {
    event_handlers_[event_name] = handler;
}

// Function checking if the structur of payload is correct
void DiscordEventHandler::handle_event(const std::string& payload, std::function<void(const std::string&)> error_handler) {
    try {
        auto json_payload = nlohmann::json::parse(payload);
        
        const auto& op_code = json_payload["op"];
        if (op_code.is_null()) {
            error_handler("Invalid op code");
            return ;
        }
        const auto& event_type = json_payload["t"];
        if (event_type.is_null()) {
            error_handler("Invalid event type");
            return ;
        }
        const auto& handler = event_handlers_.find(event_type);
        if (handler != event_handlers_.end()) {
            handler->second(json_payload["d"]);
        } else {
            error_handler("Unhandled event: " + event_type.get<std::string>());
        }
    } catch (const nlohmann::json::parse_error& e) {
        error_handler("JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        error_handler("Unknown error: " + std::string(e.what()));
    }
}

} // namespace discord
