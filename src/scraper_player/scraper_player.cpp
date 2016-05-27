#include <iostream>
#include <stream/string_reader.h>
#include <string-utils/utils.h>
#include <unistd.h>

#include "scraper_player.h"
#include "header_reader.h"

ScraperPlayer::ScraperPlayer(Socket &radioSocket, IOEvents &events,
                             std::string streamPath, bool metadata)
        : radioSocket(radioSocket), events(events), streamPath(streamPath),
          metadata(metadata) {
    this->reader = new HeaderReader(this->radioSocket);
    this->reading = Reading::HEADER;
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
        switch (this->reading) {
            case Reading::HEADER:
                this->metadataInterval = ((HeaderReader*)this->reader)->getMetadataInterval();

                this->reading = Reading::STREAM;
                delete this->reader;
                this->reader = getStreamChunkReader();

                break;
            case Reading::STREAM:
                std::string s = ((StringReader*)this->reader)->getValue();
                write(fileno(stdout), s.c_str(), s.length());

                delete this->reader;
                this->reader = getStreamChunkReader();

                break;
        }
    }
}

std::string ScraperPlayer::getRequest() {
    std::string type = "GET " + this->streamPath + " HTTP/1.0" + this->EOL;
    std::string metadataFlag = (this->metadata) ? "1" : "0";
    std::string metadataTag = "Icy-Metadata:" + metadataFlag + this->EOL;

    return type + metadataTag + this->EOL;
}

Reader *ScraperPlayer::getStreamChunkReader() {
    return new StringReader(this->radioSocket, [&](std::string s) {
        return s.length() == 1000;
    });
}