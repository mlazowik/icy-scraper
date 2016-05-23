#ifndef CHAT_NUMBER_PARSER_H
#define CHAT_NUMBER_PARSER_H

#include "parser.h"
#include "string_parser.h"

class NumberParser : public Parser {
public:
    NumberParser(StringParser stringParser);
    void parse();
    int getValue() const;

private:
    StringParser stringParser;
    int value;
};

#endif //CHAT_NUMBER_PARSER_H
