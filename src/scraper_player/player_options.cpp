#include <stdexcept>
#include <memory>

#include "player_options.h"

PlayerOptions::PlayerOptions(const std::vector<std::shared_ptr<Parser>> parsers)
        : Options(parsers, 6) { }

std::string PlayerOptions::getUsage() const {
    return "Usage: player host path r-port file m-port md";
}