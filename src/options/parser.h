#ifndef CHAT_PARSER_H
#define CHAT_PARSER_H

class Parser {
public:
    virtual ~Parser() {};
    virtual void parse() = 0;
};

#endif //CHAT_PARSER_H
