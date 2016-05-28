#include <iostream>
#include <stream/string_reader.h>
#include <unistd.h>

#include "scraper_player.h"
#include "header_reader.h"
#include "metadata_reader.h"

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
        std::string s, metadata;

        switch (this->reading) {
            case Reading::HEADER:
                this->metadataInterval = ((HeaderReader*)this->reader)->getMetadataInterval();
                this->leftTillMetadata = this->metadataInterval;

                this->reading = Reading::STREAM;
                delete this->reader;
                this->reader = getStreamChunkReader();

                break;
            case Reading::STREAM:
                s = ((StringReader*)this->reader)->getValue();
                write(fileno(stdout), s.c_str(), s.length());

                delete this->reader;

                if (this->metadataInterval > 0) {
                    this->leftTillMetadata -= this->chunkLength;
                }

                if (this->metadataInterval > 0 && this->leftTillMetadata == 0) {
                    this->reader = this->getMetadataReader();
                    this->reading = Reading::METADATA;
                } else {
                    this->reader = getStreamChunkReader();
                    this->reading = Reading::STREAM;
                }

                break;
            case Reading::METADATA:
                metadata = ((MetadataReader*)this->reader)->getMetadata();
                if (metadata.length() > 0) {
                    std::cerr << metadata << "\n";
                }

                delete this->reader;

                this->leftTillMetadata = this->metadataInterval;
                this->reader = getStreamChunkReader();
                this->reading = Reading::STREAM;

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
    this->chunkLength = 1000;

    if (this->metadataInterval > 0) {
        this->chunkLength = std::min(this->chunkLength, this->leftTillMetadata);
    }

    return new StringReader(this->radioSocket, [&](std::string s) {
        return s.length() == this->chunkLength;
    });
}

Reader *ScraperPlayer::getMetadataReader() {
    return new MetadataReader(this->radioSocket);
}