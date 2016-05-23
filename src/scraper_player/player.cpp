#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "player_options.h"
#include "scraper_player.h"
#include "../common/socket.h"

int main(int argc, char* argv[]) {
    const int DEFAULT_PORT = 20160;

    std::vector<std::string> arguments;

    for (int argument = 0; argument < argc; argument++) {
        arguments.push_back(argv[argument]);
    }

    PlayerOptions options(arguments, DEFAULT_PORT);

    try {
        options.parse();
    } catch(std::exception &ex) {
        std::cerr << "Invalid arguments: " << ex.what() << "\n";
        std::cerr << options.getUsage() << "\n";
        exit(EXIT_FAILURE);
    }

    int port = options.getPort();
    std::string host = options.getHost();

    Socket server;

    try {
        server.setPort(port);
        server.setHost(host);

        server.connect();

        IOEvents events(2);

        ScraperPlayer client(server, events);
        client.run();
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}