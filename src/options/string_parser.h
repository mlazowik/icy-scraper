#ifndef CHAT_STRING_PARSER_H
#define CHAT_STRING_PARSER_H

#include <string>
#include "parser.h"

class StringParser : public Parser {
public:
    StringParser(char *raw);
    void parse();
    std::string getValue() const;

private:
    char *raw;
    std::string value;
};

#endif //CHAT_STRING_PARSER_H
