#ifndef CHAT_SOCKET_H
#define CHAT_SOCKET_H

#include <string>
#include <netdb.h>

#include "stream.h"
#include "descriptor.h"

class Socket : public Stream, public Desciptor {
public:
    Socket();
    Socket(int descriptor);

    int getDescriptor() const;
    void destroy();

    void setPort(int port);
    void setHost(std::string host);

    void connect();

    void bindToAddress();
    void startListening();
    Socket acceptConnection();

    bool operator==(const Desciptor &rhs) const;
    bool operator!=(const Desciptor &rhs) const;
    bool operator<(const Desciptor &rhs) const;
    bool operator>(const Desciptor &rhs) const;

    size_t getChunk(void *buffer, size_t bufferSize) const;
    void sendChunk(const void *buffer, size_t bufferSize) const;

private:
    int descriptor;
    int port;

    struct addrinfo *address;

    const int BACKLOG_QUEUE_LENGTH = 5;
};



#endif //CHAT_SOCKET_H
