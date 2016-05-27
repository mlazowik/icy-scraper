#include <iostream>
#include <vector>

#include <options/string_parser.h>
#include <options/number_parser.h>
#include <options/boolean_parser.h>
#include <networking/socket.h>
#include <stream/string_reader.h>
#include <io/io_events.h>

#include "player_options.h"

const std::string EOL = "\r\n";

bool endsWith(const std::string &string, const std::string &suffix) {
    if (string.length() < suffix.length()) {
        return false;
    }

    return std::equal(suffix.rbegin(), suffix.rend(), string.rbegin());
}

StringReader* getLineReader(Socket &socket) {
    return new StringReader(socket, [&](std::string s) {
        return endsWith(s, EOL);
    });
}

std::string getRequest(std::string path, bool pullMetadata) {
    std::string type = "GET " + path + " HTTP/1.0" + EOL;
    std::string metadataFlag = (pullMetadata) ? "1" : "0";
    std::string metadataTag = "Icy-Metadata:" + metadataFlag + EOL;

    return type + metadataTag + EOL;
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

    std::cout << "Radio: http://" << radioHost->getValue() << ":" <<
            radioPort->getValue() << streamPath->getValue() << ", metadata: " <<
            metadata->getValue() << "\n";
    std::cout << "File: " << file->getValue() << "\n";
    std::cout << "Control port: " << controlPort->getValue() << "\n";

    Socket radio;

    IOEvents events(2);

    StringReader *reader;

    try {
        radio.setPort(radioPort->getValue());
        radio.setHost(radioHost->getValue());

        radio.connect();

        std::string request = getRequest(streamPath->getValue(), metadata->getValue());

        radio.sendChunk(request.c_str(), request.length());

        reader = getLineReader(radio);

        events.registerDescriptor(&radio, [&](Descriptor *socket, short revents) {
            reader->readNextChunk();

            if (reader->finished()) {
                std::cerr << reader->getValue();

                if (reader->getValue() == "\r\n") {
                    events.removeDescriptor(&radio);
                    delete reader;
                } else {
                    delete reader;
                    reader = getLineReader(radio);
                }
            }
        });

        while (true) {
            events.processEvents();
        }
    } catch (std::exception &ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}