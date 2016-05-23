#ifndef CHAT_CLIENT_OPTIONS_H
#define CHAT_CLIENT_OPTIONS_H

#include <string>
#include <vector>
#include <memory>
#include <options/options.h>

class PlayerOptions : public Options {
public:
    PlayerOptions(std::vector<std::shared_ptr<Parser>> parsers);
    std::string getUsage() const;
};

#endif //CHAT_CLIENT_OPTIONS_H
