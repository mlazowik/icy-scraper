#include <stream/string_reader.h>
#include <string-utils/utils.h>

#include "header_reader.h"

HeaderReader::HeaderReader(Stream &stream) : stream(stream) {
    this->reader = this->getLineReader();
}

void HeaderReader::readNextChunk() {
    if (this->finished()) {
        throw std::logic_error("cannot read: already finished");
    }

    this->reader->readNextChunk();

    if (reader->finished()) {
        this->currentLine = ((StringReader*)reader)->getValue();
        delete this->reader;

        if (String::startsWith(this->currentLine, this->metadataIntervalHeader)) {
            this->parseMetadataHeader();
        }

        if (this->currentLine == EOL) {
            this->readEmptyLine = true;
        } else {
            this->reader = getLineReader();
        }
    }
}

int HeaderReader::getMetadataInterval() {
    return this->metadataInterval;
}

void HeaderReader::parseMetadataHeader() {
    std::string numberString = this->currentLine.substr(
            this->metadataIntervalHeader.length(),
            this->currentLine.length() - this->metadataIntervalHeader.length() - 2
    );

    this->metadataInterval = String::toInt(numberString);
}

bool HeaderReader::finished() const {
    return this->readEmptyLine;
}

Reader* HeaderReader::getLineReader() {
    return new StringReader(this->stream, [&](std::string s) {
        return String::endsWith(s, this->EOL);
    });
}