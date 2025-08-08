#include "Lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& source) : src(source) {}

char Lexer::peek() const {
    return pos < src.size() ? src[pos] : '\0';
}

char Lexer::advance() {
    if (pos < src.size()) {
        char c = src[pos++];
        column++;
        if (c == '\n') { line++; column = 1; }
        return c;
    }
    return '\0';
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column);
}

Token Lexer::identifier() {
    size_t start = pos - 1;
    while (isalnum(peek()) || peek() == '_') advance();
    std::string lexeme = src.substr(start, pos - start);
    return makeToken(TokenType::Identifier, lexeme);
}

Token Lexer::number() {
    size_t start = pos - 1;
    while (isdigit(peek())) advance();
    if (peek() == '.') {
        advance();
        while (isdigit(peek())) advance();
        return makeToken(TokenType::Float, src.substr(start, pos - start));
    }
    return makeToken(TokenType::Number, src.substr(start, pos - start));
}

Token Lexer::string() {
    std::string result;
    while (peek() != '"' && peek() != '\0') {
        result += advance();
    }
    if (peek() == '"') advance();
    return makeToken(TokenType::String, result);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < src.size()) {
        skipWhitespace();
        char c = advance();

        if (isalpha(c)) {
            tokens.push_back(identifier());
        } else if (isdigit(c)) {
            tokens.push_back(number());
        } else if (c == '"') {
            tokens.push_back(string());
        } else {
            switch (c) {
                case '(': tokens.push_back(makeToken(TokenType::OpenParen, "(")); break;
                case ')': tokens.push_back(makeToken(TokenType::CloseParen, ")")); break;
                case '{': tokens.push_back(makeToken(TokenType::OpenBrace, "{")); break;
                case '}': tokens.push_back(makeToken(TokenType::CloseBrace, "}")); break;
                case ':': tokens.push_back(makeToken(TokenType::Colon, ":")); break;
                case ';': tokens.push_back(makeToken(TokenType::Semicolon, ";")); break;
                case ',': tokens.push_back(makeToken(TokenType::Comma, ",")); break;
                case '=': tokens.push_back(makeToken(TokenType::Equals, "=")); break;
                default:
                    tokens.push_back(makeToken(TokenType::Unknown, std::string(1, c)));
            }
        }
    }

    tokens.push_back(makeToken(TokenType::EndOfFile, "<EOF>"));
    return tokens;
}
