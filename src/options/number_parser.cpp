#include <stdexcept>

#include "number_parser.h"

NumberParser::NumberParser(StringParser stringParser)
        : stringParser(stringParser) { }

void NumberParser::parse() {
    this->stringParser.parse();

    std::string numberString = this->stringParser.getValue();
    std::string::size_type first_after_number;

    try {
        this->value = std::stoi(numberString, &first_after_number);
    } catch (std::invalid_argument &ex) {
        throw std::invalid_argument(numberString + " is not a number");
    } catch (std::out_of_range &ex) {
        throw std::out_of_range(numberString + " is out of range");
    }

    if (first_after_number != numberString.length()) {
        throw std::invalid_argument(numberString + " is not a number");
    }
}

int NumberParser::getValue() const {
    return this->value;
}