#include "string_parser.h"

StringParser::StringParser(char *raw) {
    this->raw = raw;
}

void StringParser::parse() {
    this->value = std::string(this->raw);
}

std::string StringParser::getValue() const {
    return this->value;
}





