#include <iostream>
#include <stream/string_reader.h>
#include <unistd.h>

#include "scraper_player.h"
#include "header_reader.h"
#include "metadata_reader.h"

ScraperPlayer::ScraperPlayer(TCPSocket &radioSocket, UDPSocket &controlSocket,
                             Descriptor &output, IOEvents &events,
                             std::string streamPath, bool metadata)
        : radioSocket(radioSocket), controlSocket(controlSocket),
          output(output), events(events), streamPath(streamPath),
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
                this->handleRadioEvent((TCPSocket*) socket, revents);
            }
    );

    this->events.registerDescriptor(
            &this->controlSocket,
            [&](Descriptor *socket, short revents) {
                this->handleControlEvent((UDPSocket*) socket, revents);
            }
    );

    while (true) {
        this->events.processEvents();
    }
}

void ScraperPlayer::handleRadioEvent(TCPSocket *socket, short revents) {
    try {
        this->reader->readNextChunk();
    } catch (stream_closed_error &ex) {
        exit(EXIT_SUCCESS);
    }

    if (reader->finished()) {
        std::string s, metadata;

        switch (this->reading) {
            case Reading::HEADER:
                this->metadataInterval = (
                        (HeaderReader*)this->reader
                )->getMetadataInterval();

                this->leftTillMetadata = this->metadataInterval;

                this->reading = Reading::STREAM;
                delete this->reader;
                this->reader = getStreamChunkReader();

                break;
            case Reading::STREAM:
                s = ((StringReader*)this->reader)->getValue();
                // TODO: use event loop for writing
                if (this->writing) {
                    write(this->output.getDescriptor(), s.c_str(), s.length());
                }

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
                    this->currentTitle = getTitle(metadata);
                }

                delete this->reader;

                this->leftTillMetadata = this->metadataInterval;
                this->reader = getStreamChunkReader();
                this->reading = Reading::STREAM;

                break;
        }
    }
}

void ScraperPlayer::handleControlEvent(UDPSocket *socket, short revents) {
    struct sockaddr_in client_address;
    socklen_t address_length = (socklen_t) sizeof(client_address);
    char buffer[11];
    int flags = 0, send_flags = 0;

    ssize_t len = recvfrom(socket->getDescriptor(),
                           buffer,
                           sizeof(buffer) - sizeof(char), flags,
                           (struct sockaddr *) &client_address,
                           &address_length);

    buffer[len] = '\0';

    std::string command(buffer);

    if (command == "PAUSE") {
        this->writing = false;
    } else if (command == "PLAY"){
        this->writing = true;
    } else if (command == "TITLE") {
        ssize_t sent_len = sendto(
                socket->getDescriptor(),
                this->currentTitle.c_str(),
                this->currentTitle.length(),
                send_flags,
                (struct sockaddr *) &client_address,
                address_length
        );

        if (sent_len != this->currentTitle.length()) {
            throw new std::runtime_error("Error while replying to TITLE");
        }
    } else if (command == "QUIT") {
        exit(EXIT_SUCCESS);
    } else {
        std::cerr << "Unknown command: " << command << "\n";
    }
}

std::string ScraperPlayer::getTitle(std::string metadata) {
    std::string title;

    std::string titleParam = "StreamTitle='";
    size_t start = metadata.find(titleParam);

    if (start == std::string::npos) {
        return title;
    }

    metadata = metadata.substr(start + titleParam.length());

    size_t end  = metadata.find("';");

    if (end == std::string::npos) {
        return title;
    }

    return metadata.substr(0, end);
}

std::string ScraperPlayer::getRequest() {
    std::string type = "GET " + this->streamPath + " HTTP/1.0" + this->EOL;
    std::string agent = "User-Agent: ICY-scraper 1.0 / Networking class @ University of Warsaw project" + this->EOL;
    std::string metadataFlag = (this->metadata) ? "1" : "0";
    std::string metadataTag = "Icy-Metadata:" + metadataFlag + this->EOL;

    return type + agent + metadataTag + this->EOL;
}

Reader *ScraperPlayer::getStreamChunkReader() {
    this->chunkLength = 10;

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