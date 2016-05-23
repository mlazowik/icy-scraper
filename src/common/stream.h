#ifndef CHAT_STREAM_H
#define CHAT_STREAM_H

#include <cstddef>
#include <stdexcept>

class stream_closed_error : public std::runtime_error {
public:
    stream_closed_error(const std::string &what)
            : runtime_error(what) {
    }
};

class Stream {
public:
    virtual ~Stream() {}
    virtual size_t getChunk(void *buffer, size_t bufferSize) const = 0;
    virtual void sendChunk(const void *buffer, size_t bufferSize) const = 0;
};

#endif //CHAT_STREAM_H
