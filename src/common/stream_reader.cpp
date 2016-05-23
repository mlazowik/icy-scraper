#include <stdexcept>
#include "stream_reader.h"

StreamReader::StreamReader(Stream &stream, void *buffer, size_t bytes)
        : stream(stream), buffer(buffer), bytes(bytes) {
    this->bufferPosition = 0;
}

void StreamReader::readNextChunk() {
    if (this->finished()) {
        throw std::logic_error("cannot read: already finished");
    }

    size_t remains = bytes - this->bufferPosition;

    size_t len = this->stream.getChunk(
            (char*)this->buffer + this->bufferPosition, remains);

    if (len == 0) {
        throw stream_closed_error("connection closed");
    }

    this->bufferPosition += len;
}

bool StreamReader::finished() const {
    return this->bufferPosition == this->bytes;
}