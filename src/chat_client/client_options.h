#ifndef CHAT_CLIENT_OPTIONS_H
#define CHAT_CLIENT_OPTIONS_H

#include <string>
#include <vector>
#include "../common/options.h"

class ClientOptions : public Options {
public:
    ClientOptions(std::vector<std::string> arguments, int defaultPort);
    void parse();
    std::string getHost() const;
    int getPort() const;
    std::string getUsage() const;

private:
    std::vector<std::string> arguments;

    int port;
    std::string host;
};

#endif //CHAT_CLIENT_OPTIONS_H
