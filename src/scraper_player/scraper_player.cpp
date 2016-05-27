#include <iostream>
#include <stream/string_reader.h>

#include "scraper_player.h"

ScraperPlayer::ScraperPlayer(Socket &radioSocket, IOEvents &events,
                             std::string streamPath, bool metadata)
        : radioSocket(radioSocket), events(events), streamPath(streamPath),
          metadata(metadata) {
    this->reader = this->getLineReader();
}

void ScraperPlayer::run() {
    std::string request = this->getRequest();

    this->radioSocket.sendChunk(request.c_str(), request.length());

    this->events.registerDescriptor(
            &this->radioSocket,
            [&](Descriptor *socket, short revents) {
                this->handleRadioEvent((Socket*) socket, revents);
            }
    );

    while (true) {
        this->events.processEvents();
    }
}

void ScraperPlayer::handleRadioEvent(Socket *socket, short revents) {
    this->reader->readNextChunk();

    if (reader->finished()) {
        std::string line = ((StringReader*)reader)->getValue();
        delete this->reader;

        std::cerr << line;

        std::string metadataIntervalHeader = "icy-metaint:";

        if (startsWith(line, metadataIntervalHeader)) {
            std::string numberString = line.substr(
                    metadataIntervalHeader.length(),
                    line.length() - metadataIntervalHeader.length() - 2
            );

            this->metadataInterval = this->toInt(numberString);
        }

        if (line == EOL) {
            events.removeDescriptor(&this->radioSocket);
            std::cerr << "Interval: " << metadataInterval << "\n";
        } else {
            this->reader = getLineReader();
        }
    }
}

Reader *ScraperPlayer::getLineReader() {
    return new StringReader(this->radioSocket, [&](std::string s) {
        return endsWith(s, EOL);
    });
}

std::string ScraperPlayer::getRequest() {
    std::string type = "GET " + this->streamPath + " HTTP/1.0" + this->EOL;
    std::string metadataFlag = (this->metadata) ? "1" : "0";
    std::string metadataTag = "Icy-Metadata:" + metadataFlag + this->EOL;

    return type + metadataTag + this->EOL;
}

bool ScraperPlayer::startsWith(const std::string &string,
                               const std::string &prefix) {
    if (string.length() < prefix.length()) {
        return false;
    }

    return std::equal(prefix.begin(), prefix.end(), string.begin());
}

bool ScraperPlayer::endsWith(const std::string &string,
                             const std::string &suffix) {
    if (string.length() < suffix.length()) {
        return false;
    }

    return std::equal(suffix.rbegin(), suffix.rend(), string.rbegin());
}

int ScraperPlayer::toInt(std::string numberString) {
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