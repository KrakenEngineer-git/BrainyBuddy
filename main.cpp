#include "DiscordBot/BrainyBuddy.hpp"
#include <iostream>

int main() {
    try {
        BrainyBuddy bot;
        bot.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
