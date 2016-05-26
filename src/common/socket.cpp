#include <system_error>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "socket.h"

Socket::Socket() {
    this->descriptor = socket(PF_INET, SOCK_STREAM, 0);

    if (this->descriptor < 0) {
        throw std::system_error(errno, std::system_category());
    }
}

int Socket::getDescriptor() const {
    return this->descriptor;
}

Socket::Socket(int descriptor) {
    this->descriptor = descriptor;
}

void Socket::setPort(int port) {
    this->port = port;
}

void Socket::setHost(std::string host) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portString = std::to_string(this->port);

    int err = getaddrinfo(host.c_str(), portString.c_str(), &hints, &this->address);
    if (err != 0) {
        if (err == EAI_SYSTEM) {
            throw std::system_error(errno, std::system_category());
        } else {
            std::string e(gai_strerror(err));
            throw std::runtime_error("failed too look up the address: " + e);
        }
    }
}

void Socket::connect() {
    if (::connect(this->descriptor, address->ai_addr, address->ai_addrlen) != 0) {
        throw std::system_error(errno, std::system_category());
    }
}

void Socket::bindToAddress() {
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(this->port);

    if (bind(this->descriptor, (struct sockaddr *) &address, sizeof(address)) < 0) {
        throw std::system_error(errno, std::system_category());
    }
}

void Socket::startListening() {
    listen(this->descriptor, this->BACKLOG_QUEUE_LENGTH);
}

Socket Socket::acceptConnection() {
    int connectionDescriptor = accept(this->descriptor, (struct sockaddr*)0,
                               (socklen_t*)0);

    if (connectionDescriptor < 0) {
        throw std::system_error(errno, std::system_category());
    }

    return Socket(connectionDescriptor);
}

void Socket::destroy() {
    if (close(this->descriptor) < 0) {
        throw std::system_error(errno, std::system_category());
    }
}

size_t Socket::getChunk(void *buffer, size_t bufferSize) const {
    ssize_t len = read(this->getDescriptor(), buffer, bufferSize);

    if (len < 0) {
        throw std::system_error(errno, std::system_category());
    }

    return (size_t)len;
}

void Socket::sendChunk(const void *buffer, size_t bufferSize) const {
    ssize_t len = write(this->getDescriptor(), buffer, bufferSize);

    if (len < 0) {
        throw std::system_error(errno, std::system_category());
    }

    if ((size_t)len != bufferSize) {
        throw std::runtime_error("partial write");
    }
}

bool Socket::operator==(const Descriptor &rhs) const {
    return this->getDescriptor() == rhs.getDescriptor();
}

bool Socket::operator!=(const Descriptor &rhs) const {
    return !(*this == rhs);
}

bool Socket::operator<(const Descriptor &rhs) const {
    return this->getDescriptor() < rhs.getDescriptor();
}

bool Socket::operator>(const Descriptor &rhs) const {
    return !(*this < rhs || *this == rhs);
}