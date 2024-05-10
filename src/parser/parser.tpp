#ifndef PARSER_TPP
#define PARSER_TPP

#include "parser.hpp"

template <typename Exception>
void Parser::expect(Token::Type expected, const Exception& exception) {
    if (currentToken_.getType() != expected)
        throw exception;

    consumeToken();
}

template <typename T, typename Exception>
T Parser::expectAndReturnValue(Token::Type expected, const Exception& exception) {
    const auto value = currentToken_.getValue();
    expect(expected, exception);
    return std::get<T>(value);
}

/// LIST = [ ELEM { ',' ELEM } ]
template <typename T, typename ElementParser>
std::vector<T> Parser::parseList(ElementParser elementParser) {
    std::vector<T> elements;

    auto element = std::invoke(elementParser, this);
    if (!element)
        return elements;

    elements.push_back(std::move(*element));

    while (currentToken_.getType() == Token::Type::CMA) {
        consumeToken();
        element = std::invoke(elementParser, this);
        if (!element)
            throw SyntaxException(currentToken_.getPosition(),
                                  "Expected element after comma");
        elements.push_back(std::move(*element));
    }
    return elements;
}

#endif
