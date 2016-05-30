#include <fcntl.h>

#include <iostream>
#include <vector>
#include <system_error>

#include <options/string_parser.h>
#include <options/number_parser.h>
#include <options/boolean_parser.h>
#include <networking/tcp_socket.h>
#include <io/io_events.h>

#include "player_options.h"
#include "scraper_player.h"

int getOutputDescriptor(std::string filename) {
    int output_descriptor;

    if (filename == "-") {
        output_descriptor = fileno(stdout);
    } else {
        output_descriptor = open(filename.c_str(), O_WRONLY | O_CREAT, 0644);

        if (output_descriptor == -1) {
            throw std::system_error(errno, std::system_category(), filename);
        }
    }

    return output_descriptor;
}

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

    if (radioPort->getValue() > 65535 || radioPort->getValue() <= 0) {
        std::cerr << radioPort->getValue() << " is not a valid port\n";
        exit(EXIT_FAILURE);
    }

    if (controlPort->getValue() > 65535 || controlPort->getValue() <= 0) {
        std::cerr << controlPort->getValue() << " is not a valid port\n";
        exit(EXIT_FAILURE);
    }

    TCPSocket radio;
    UDPSocket control;

    IOEvents events(2);

    try {
        Descriptor output(getOutputDescriptor(file->getValue()));

        control.setPort(controlPort->getValue());
        control.bindToAddress();

        radio.setPort(radioPort->getValue());
        radio.setHost(radioHost->getValue());

        radio.connect();

        ScraperPlayer player(radio, control, output, events,
                             streamPath->getValue(), metadata->getValue());

        player.run();
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}