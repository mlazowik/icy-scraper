#include <stdexcept>

#include "client_options.h"

ClientOptions::ClientOptions(std::vector<std::string> arguments, int defaultPort) {
    this->arguments = arguments;

    this->port = defaultPort;
}

void ClientOptions::parse() {
    if (this->arguments.size() < 2 || this->arguments.size() > 3) {
        throw std::invalid_argument("invalid argument count");
    }

    this->host = this->arguments[1];

    if (this->arguments.size() == 3) {
        std::string port_string = this->arguments[2];

        std::string::size_type first_after_number;
        try {
            this->port = std::stoi(port_string, &first_after_number);
        } catch (std::invalid_argument &ex) {
            throw std::invalid_argument(port_string + " is not a number");
        } catch (std::out_of_range &ex) {
            throw std::out_of_range(port_string + " is out of range");
        }
        if (first_after_number != port_string.length()) {
            throw std::invalid_argument(port_string + " is not a number");
        }
    }
}

int ClientOptions::getPort() const {
    return this->port;
}

std::string ClientOptions::getHost() const {
    return this->host;
}

std::string ClientOptions::getUsage() const {
    return "Usage: chat_client host [PORT]";
}