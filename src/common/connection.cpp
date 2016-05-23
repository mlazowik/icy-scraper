#include <stdexcept>
#include <netinet/in.h>

#include "connection.h"

Connection::Connection() {}

Connection::Connection(Socket &socket) : socket(socket) {
    this->reading = Reading::NOTHING;
    this->messageLength = new uint16_t;
    this->buffer = new char[this->BUFFER_LENGTH];
}

Connection::~Connection() {
    delete[] this->messageLength;
    delete[] this->buffer;
}

int Connection::getDescriptor() const {
    return this->socket.getDescriptor();
}

void Connection::destroy() {
    this->socket.destroy();
}

void Connection::read() {
    if (finishedReading()) {
        this->reading = Reading::LENGTH;
        this->reader = this->getLengthReader();
    }

    this->reader->readNextChunk();

    if (this->reader->finished()) {
        switch (this->reading) {
            case Reading::LENGTH:
                *this->messageLength = ntohs(*this->messageLength);
                delete this->reader;

                if (*this->messageLength > 1000) {
                    throw invalid_message_error("message is too long");
                } else if (*this->messageLength == 0) {
                    this->reading = Reading::NOTHING;
                    this->buffer[*this->messageLength] = '\0';
                } else {
                    this->reader = getMessageReader();
                    this->reading = Reading::MESSAGE;
                }

                break;
            case Reading::MESSAGE:
                delete this->reader;
                this->reading = Reading::NOTHING;
                this->buffer[*this->messageLength] = '\0';
                break;
            case Reading::NOTHING:
                throw std::logic_error("we cannot be reading nothing here!");
        }
    }
}

bool Connection::finishedReading() const {
    return this->reading == Reading::NOTHING;
}

std::string Connection::getMessage() const {
    return std::string(this->buffer);
}

void Connection::sendMessage(std::string message) const {
    uint16_t len = htons(message.length());

    this->socket.sendChunk((char*)&len, sizeof(len));
    this->socket.sendChunk(message.c_str(), message.length());
}

StreamReader* Connection::getLengthReader() {
    return new StreamReader(this->socket, (char*)this->messageLength,
                            sizeof(*this->messageLength));
}

StreamReader* Connection::getMessageReader() {
    return new StreamReader(this->socket, this->buffer,
                            *this->messageLength);
}

bool Connection::operator==(const Desciptor &rhs) const {
    return this->getDescriptor() == rhs.getDescriptor();
}

bool Connection::operator!=(const Desciptor &rhs) const {
    return !(*this == rhs);
}

bool Connection::operator<(const Desciptor &rhs) const {
    return this->getDescriptor() < rhs.getDescriptor();
}

bool Connection::operator>(const Desciptor &rhs) const {
    return !(*this < rhs || *this == rhs);
}