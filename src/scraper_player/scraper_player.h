#ifndef CHAT_CHAT_CLIENT_H
#define CHAT_CHAT_CLIENT_H

#include "../common/socket.h"
#include "../common/io_events.h"

class ScraperPlayer {
public:
    ScraperPlayer(Socket &serverSocket, IOEvents &events);
    ~ScraperPlayer();
    void run();

private:
    void handleServerEvent(Connection *connection, short revents);
    void handleClientEvent(Connection *connection, short revents);

    void disconnectServer(Connection *connection);

    Socket serverSocket;
    Socket *clientSocket;
    IOEvents events;

    const int BUFFER_SIZE = 1024;

    bool lastEndedWithNewline = true;

    Connection *server;
    Connection *client;
};

#endif //CHAT_CHAT_CLIENT_H
