#include <iostream>
#include <stream/primitive_type_reader.h>
#include <stream/string_reader.h>
#include "metadata_reader.h"

MetadataReader::MetadataReader(Socket &socket) : socket(socket) {
    this->reader = this->getMetadataLengthReader();
    this->reading = Reading::LENGTH;
}

void MetadataReader::readNextChunk() {
    if (this->finished()) {
        throw std::logic_error("cannot read: already finished");
    }

    this->reader->readNextChunk();

    if (this->reader->finished()) {
        switch (this->reading) {
            case Reading::LENGTH:
                this->metadataLength = ((PrimitiveTypeReader<u_int8_t>*)this->reader)->getValue();
                this->metadataLength *= 16;

                delete this->reader;

                if (this->metadataLength == 0) {
                    this->reading = Reading::NOTHING;
                } else {
                    this->reading = Reading::METADATA;
                    this->reader = this->getMetadataReader();
                }

                break;
            case Reading::METADATA:
                this->metadata = ((StringReader*)this->reader)->getValue();
                delete this->reader;
                this->reading = Reading::NOTHING;

                break;
            case Reading::NOTHING:
                throw new std::logic_error("We cannot be reading nothing here!");
        }
    }
}

bool MetadataReader::finished() const {
    return this->reading == Reading::NOTHING;
}

std::string MetadataReader::getMetadata() {
    return this->metadata;
}

Reader* MetadataReader::getMetadataLengthReader() {
    return new PrimitiveTypeReader<u_int8_t>(this->socket);
}

Reader* MetadataReader::getMetadataReader() {
    return new StringReader(this->socket, [&](std::string s) {
        return s.length() == this->metadataLength;
    });
}