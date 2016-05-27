#ifndef CHAT_SCRAPER_PLAYER_H
#define CHAT_SCRAPER_PLAYER_H

#include <networking/socket.h>
#include <io/io_events.h>
#include <stream/reader.h>

class ScraperPlayer {
public:
    ScraperPlayer(Socket &radioSocket, IOEvents &events, std::string streamPath,
                  bool metadata);
    void run();

private:
    const std::string EOL = "\r\n";

    bool startsWith(const std::string &string, const std::string &prefix);
    bool endsWith(const std::string &string, const std::string &suffix);

    int toInt(std::string numberString);

    std::string getRequest();
    void handleRadioEvent(Socket *socket, short revents);
    Reader* getLineReader();

    int metadataInterval = 0;

    Socket radioSocket;
    IOEvents events;
    std::string streamPath;
    bool metadata;

    Reader *reader;
};

#endif //CHAT_SCRAPER_PLAYER_H
