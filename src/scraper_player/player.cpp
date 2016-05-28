#include <iostream>
#include <vector>

#include <options/string_parser.h>
#include <options/number_parser.h>
#include <options/boolean_parser.h>
#include <networking/socket.h>
#include <io/io_events.h>

#include "player_options.h"
#include "scraper_player.h"

int main(int argc, char* argv[]) {
    auto radioHost = std::make_shared<StringParser>(argv[1]);
    auto streamPath = std::make_shared<StringParser>(argv[2]);
    auto radioPort = std::make_shared<NumberParser>(StringParser(argv[3]));
    auto file = std::make_shared<StringParser>(argv[4]);
    auto controlPort = std::make_shared<NumberParser>(StringParser(argv[5]));
    auto metadata = std::make_shared<BooleanParser>(StringParser(argv[6]));

    PlayerOptions options({radioHost, streamPath, radioPort, file,
                           controlPort, metadata});

    try {
        options.parse((size_t)argc - 1);
    } catch(std::exception &ex) {
        std::cerr << "Invalid arguments: " << ex.what() << "\n";
        std::cerr << options.getUsage() << "\n";
        exit(EXIT_FAILURE);
    }

    std::cerr << "Radio: http://" << radioHost->getValue() << ":" <<
            radioPort->getValue() << streamPath->getValue() << ", metadata: " <<
            metadata->getValue() << "\n";
    std::cerr << "File: " << file->getValue() << "\n";
    std::cerr << "Control port: " << controlPort->getValue() << "\n";

    Socket radio;

    IOEvents events(2);

    try {
        radio.setPort(radioPort->getValue());
        radio.setHost(radioHost->getValue());

        radio.connect();

        ScraperPlayer player(radio, events, streamPath->getValue(),
                             metadata->getValue());

        player.run();
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}