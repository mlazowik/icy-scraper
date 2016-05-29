#ifndef CHAT_SCRAPER_PLAYER_H
#define CHAT_SCRAPER_PLAYER_H

#include <networking/tcp_socket.h>
#include <io/io_events.h>
#include <stream/reader.h>
#include <networking/udp_socket.h>

class ScraperPlayer {
public:
    ScraperPlayer(TCPSocket &radioSocket, UDPSocket &controlSocket,
                  IOEvents &events, std::string streamPath, bool metadata);
    void run();

private:
    const std::string EOL = "\r\n";

    std::string getRequest();
    void handleRadioEvent(TCPSocket *socket, short revents);
    void handleControlEvent(UDPSocket *socket, short revents);

    size_t metadataInterval = 0;
    size_t leftTillMetadata;
    size_t chunkLength;

    std::string getTitle(std::string metadata);

    TCPSocket radioSocket;
    UDPSocket controlSocket;
    IOEvents events;
    std::string streamPath;
    bool metadata;
    bool writing = true;
    std::string currentTitle;

    enum class Reading {
        HEADER,
        STREAM,
        METADATA
    } reading;

    Reader* getStreamChunkReader();
    Reader* getMetadataReader();

    Reader *reader;
};

#endif //CHAT_SCRAPER_PLAYER_H
