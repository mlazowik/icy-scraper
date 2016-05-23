#include <cstdlib>
#include <iostream>
#include <cstring>
#include <system_error>

#include "scraper_player.h"

ScraperPlayer::ScraperPlayer(Socket &serverSocket, IOEvents &events)
        : serverSocket(serverSocket), events(events) {
    this->clientSocket = new Socket(fileno(stdin));
}

ScraperPlayer::~ScraperPlayer() {
    delete this->clientSocket;
}

void ScraperPlayer::run() {
    this->server = new Connection(this->serverSocket);
    this->client = new Connection(*this->clientSocket);

    this->events.registerConnection(
            this->server,
            [&](Connection *connection, short revents) {
                this->handleServerEvent(connection, revents);
            }
    );

    this->events.registerConnection(
            this->client,
            [&](Connection *connection, short revents) {
                this->handleClientEvent(connection, revents);
            }
    );

    while (true) {
        this->events.processEvents();
    }
}

void ScraperPlayer::handleServerEvent(Connection *connection, short revents) {
    if (!(revents & (POLLIN | POLLHUP))) return;

    try {
        connection->read();
    } catch (invalid_message_error &ex) {
        std::cerr << "Invalid message\n";
        this->disconnectServer(connection);
        exit(EXIT_FAILURE);
    } catch (stream_closed_error &ex) {
        std::cerr << "Server closed connection\n";
        this->disconnectServer(connection);
        exit(EXIT_FAILURE);
    } catch (std::system_error &ex) {
        this->disconnectServer(connection);
        exit(EXIT_FAILURE);
    }

    if (connection->finishedReading()) {
        std::string message = connection->getMessage();
        std::cout << message << "\n";
        fflush(stdout);
    }
}

void ScraperPlayer::handleClientEvent(Connection *connection, short revents) {
    if (!(revents & (POLLIN | POLLHUP))) return;

    char buffer[this->BUFFER_SIZE];

    if (std::fgets(buffer, 1001, stdin) != NULL) {
        size_t lengthWithoutNewline = std::strcspn(buffer, "\n");

        if (!this->lastEndedWithNewline && lengthWithoutNewline == 0) {
            return;
        }
        this->lastEndedWithNewline = lengthWithoutNewline != 1000;
        buffer[lengthWithoutNewline] = '\0';
        std::string message(buffer);
        this->server->sendMessage(message);
    } else {
        if (std::feof(stdin)) {
            this->disconnectServer(server);
            exit(EXIT_SUCCESS);
        }
    }
}

void ScraperPlayer::disconnectServer(Connection *connection) {
    this->events.deregisterConnection(connection);
    connection->destroy();
    delete connection;
}