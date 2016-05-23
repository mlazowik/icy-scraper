#include "boolean_parser.h"

BooleanParser::BooleanParser(StringParser stringParser)
        : stringParser(stringParser) { }

void BooleanParser::parse() {
    this->stringParser.parse();

    std::string booleanString = this->stringParser.getValue();

    if (this->mappings.find(booleanString) == this->mappings.end()) {
        throw std::invalid_argument(
                booleanString + " is not a boolean. "
                + "Valid values are: " + this->getValidValues()
        );
    }

    this->value = this->mappings[booleanString];
}

bool BooleanParser::getValue() const {
    return this->value;
}

std::string BooleanParser::getValidValues() const {
    std::string validValues = "";

    for (auto it = this->mappings.begin(); it != this->mappings.end(); it++) {
        if (it != this->mappings.begin()) {
            validValues += ", ";
        }
        validValues += it->first;
    }

    return validValues;
}