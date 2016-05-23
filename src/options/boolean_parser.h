#ifndef CHAT_BOOLEAN_PARSER_H
#define CHAT_BOOLEAN_PARSER_H

#include <map>
#include "parser.h"
#include "string_parser.h"

class BooleanParser : public Parser {
public:
    BooleanParser(StringParser stringParser);
    void parse();
    bool getValue() const;

private:
    std::string getValidValues() const;

    StringParser stringParser;
    bool value;

    std::map<std::string, bool> mappings = {
            {"yes", true},
            {"no", false}
    };
};

#endif //CHAT_BOOLEAN_PARSER_H
