#ifndef CHAT_STREAM_READER_H
#define CHAT_STREAM_READER_H

#include <cstdint>
#include <string>

#include "stream.h"

class StreamReader {
public:
    StreamReader(Stream &stream, void *buffer, size_t bytes);
    void readNextChunk();
    bool finished() const;

private:
    Stream &stream;
    void *buffer;
    size_t bytes;
    size_t bufferPosition;
};

#endif //CHAT_STREAM_READER_H
