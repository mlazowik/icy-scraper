#ifndef CHAT_OPTIONS_H
#define CHAT_OPTIONS_H

#include <string>
#include <vector>
#include <memory>
#include "parser.h"

class Options {
public:
    Options(std::vector<std::shared_ptr<Parser>> parsers,
            size_t requiredArguments);
    void parse(size_t argumentCount);
    virtual std::string getUsage() const = 0;

private:
    size_t requiredArguments;
    std::vector<std::shared_ptr<Parser>> parsers;
};

#endif //CHAT_OPTIONS_H