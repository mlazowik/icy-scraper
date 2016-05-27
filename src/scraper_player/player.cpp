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

int toInt(std::string numberString) {
    int value;
    std::string::size_type first_after_number;

    try {
        value = std::stoi(numberString, &first_after_number);
    } catch (std::invalid_argument &ex) {
        throw std::invalid_argument(numberString + " is not a number");
    } catch (std::out_of_range &ex) {
        throw std::out_of_range(numberString + " is out of range");
    }

    if (first_after_number != numberString.length()) {
        throw std::invalid_argument(numberString + " is not a number");
    }

    return value;
}

bool startsWith(const std::string &string, const std::string &prefix) {
    if (string.length() < prefix.length()) {
        return false;
    }

    return std::equal(prefix.begin(), prefix.end(), string.begin());
}

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

        int metadataInterval = 0;

        events.registerDescriptor(&radio, [&](Descriptor *socket, short revents) {
            reader->readNextChunk();

            if (reader->finished()) {
                std::string line = reader->getValue();
                delete reader;

                std::cerr << line;

                std::string metadataIntervalHeader = "icy-metaint:";

                if (startsWith(line, metadataIntervalHeader)) {
                    std::string numberString = line.substr(
                            metadataIntervalHeader.length(),
                            line.length() - metadataIntervalHeader.length() - 2
                    );

                    metadataInterval = toInt(numberString);
                }

                if (line == EOL) {
                    events.removeDescriptor(&radio);
                    std::cerr << "Interval: " << metadataInterval << "\n";
                } else {
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