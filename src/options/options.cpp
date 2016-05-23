#include <memory>
#include "options.h"

Options::Options(std::vector<std::shared_ptr<Parser>> parsers,
                 size_t requiredArguments)
        : requiredArguments(requiredArguments), parsers(parsers) { }

void Options::parse(size_t argumentCount) {
    if (argumentCount < requiredArguments || argumentCount > parsers.size()) {
        throw std::invalid_argument("invalid argument count");
    }

    for (size_t i = 0; i < argumentCount; i++) {
        this->parsers[i]->parse();
    }
}
