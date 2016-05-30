#ifndef CHAT_HEADER_READER_H
#define CHAT_HEADER_READER_H

#include <stream/reader.h>
#include <stream/stream.h>

class HeaderReader : public Reader {
public:
    HeaderReader(Stream &stream);
    void readNextChunk();
    bool finished() const;

    int getMetadataInterval();

private:
    const std::string EOL = "\r\n";
    const std::string metadataIntervalHeader = "icy-metaint:";

    Stream &stream;

    std::string currentLine;

    int metadataInterval = 0;
    bool metadataIntervalPresent = false;

    bool firstLine = true;

    bool readEmptyLine = false;

    int parseMetadataHeader();

    Reader* getLineReader();

    Reader *reader;
};

#endif //CHAT_HEADER_READER_H
