#pragma once
#include <string>
#include <unordered_set>

enum class TokenType {
    Identifier, Number, Float, String,
    // keywords
    KwCapsule, KwFunc, KwStruct, KwClass, KwLet, KwReturn,
    KwIf, KwElse, KwLoop, KwFrom, KwTo, KwSay,
    KwEnd,
    // symbols
    LParen, RParen, LBrace, RBrace, Colon, Semicolon, Comma,
    // operators
    Plus, Minus, Star, Slash, Percent, Caret,
    Bang, Amp, Pipe, Tilde,
    Less, Greater, Equal,
    LessEq, GreaterEq, EqEq, BangEq,
    AndAnd, OrOr,
    PlusEq, MinusEq, StarEq, SlashEq,
    Assign,
    EndOfFile, Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
