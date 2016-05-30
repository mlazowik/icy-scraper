#include <stream/string_reader.h>
#include <string-utils/utils.h>
#include <iostream>

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

        if (this->firstLine) {
            if (this->currentLine != "ICY 200 OK\r\n") {
                std::cerr << "not an icy server\n";
                exit(EXIT_FAILURE);
            }

            this->firstLine = false;
        }

        if (String::startsWith(this->currentLine, this->metadataIntervalHeader)) {
            this->metadataIntervalPresent = true;
            this->metadataInterval = this->parseMetadataHeader();
            if (this->metadataInterval <= 0) {
                throw new std::invalid_argument("metadata interval must be positive");
            }
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

int HeaderReader::parseMetadataHeader() {
    std::string numberString = this->currentLine.substr(
            this->metadataIntervalHeader.length(),
            this->currentLine.length() - this->metadataIntervalHeader.length() - 2
    );

    // TODO: (unsigned) long long?
    return String::toInt(numberString);
}

bool HeaderReader::finished() const {
    return this->readEmptyLine;
}

Reader* HeaderReader::getLineReader() {
    return new StringReader(this->stream, [&](std::string s) {
        return String::endsWith(s, this->EOL);
    });
}