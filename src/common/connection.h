#ifndef CHAT_CONNECTION_H
#define CHAT_CONNECTION_H

#include "socket.h"
#include "stream_reader.h"
#include "descriptor.h"

class invalid_message_error : public std::runtime_error {
public:
    invalid_message_error(const std::string &what)
            : runtime_error(what) {
    }
};

class Connection : public Desciptor {
public:
    Connection();
    Connection(Socket &socket);
    ~Connection();

    int getDescriptor() const;
    void destroy();

    bool operator==(const Desciptor &rhs) const;
    bool operator!=(const Desciptor &rhs) const;
    bool operator<(const Desciptor &rhs) const;
    bool operator>(const Desciptor &rhs) const;

    void read();
    bool finishedReading() const;
    std::string getMessage() const;
    void sendMessage(std::string message) const;

private:
    Socket socket;
    StreamReader *reader;

    const size_t BUFFER_LENGTH = 1024;

    uint16_t *messageLength;
    char *buffer;

    enum class Reading {
        LENGTH,
        MESSAGE,
        NOTHING
    } reading;

    StreamReader* getLengthReader();
    StreamReader* getMessageReader();
};

#endif //CHAT_CONNECTION_H
