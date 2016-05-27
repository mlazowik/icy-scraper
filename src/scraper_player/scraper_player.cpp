#include <iostream>
#include <stream/string_reader.h>
#include <string-utils/utils.h>

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

        if (String::startsWith(line, metadataIntervalHeader)) {
            std::string numberString = line.substr(
                    metadataIntervalHeader.length(),
                    line.length() - metadataIntervalHeader.length() - 2
            );

            this->metadataInterval = String::toInt(numberString);
        }

        if (line == EOL) {
            this->events.removeDescriptor(&this->radioSocket);
            std::cerr << "Interval: " << metadataInterval << "\n";
        } else {
            this->reader = getLineReader();
        }
    }
}

Reader *ScraperPlayer::getLineReader() {
    return new StringReader(this->radioSocket, [&](std::string s) {
        return String::endsWith(s, EOL);
    });
}

std::string ScraperPlayer::getRequest() {
    std::string type = "GET " + this->streamPath + " HTTP/1.0" + this->EOL;
    std::string metadataFlag = (this->metadata) ? "1" : "0";
    std::string metadataTag = "Icy-Metadata:" + metadataFlag + this->EOL;

    return type + metadataTag + this->EOL;
}