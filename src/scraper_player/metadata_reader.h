#ifndef CHAT_METADATA_READER_H
#define CHAT_METADATA_READER_H

#include <stream/reader.h>
#include <networking/socket.h>

class MetadataReader : public Reader {
public:
    MetadataReader(Socket &socket);
    void readNextChunk();
    bool finished() const;

    std::string getMetadata();

private:
    Socket &socket;

    u_int8_t metadataLength;
    std::string metadata;

    Reader* getMetadataLengthReader();
    Reader* getMetadataReader();

    enum class Reading {
        LENGTH,
        METADATA,
        NOTHING
    } reading;

    Reader *reader;
};

#endif //CHAT_METADATA_READER_H
