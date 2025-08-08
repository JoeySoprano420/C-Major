// ... includes
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> KW = {
    {"capsule", TokenType::KwCapsule},
    {"func", TokenType::KwFunc},
    {"struct", TokenType::KwStruct},
    {"class", TokenType::KwClass},
    {"let", TokenType::KwLet},
    {"return", TokenType::KwReturn},
    {"if", TokenType::KwIf},
    {"else", TokenType::KwElse},
    {"loop", TokenType::KwLoop},
    {"from", TokenType::KwFrom},
    {"to", TokenType::KwTo},
    {"say", TokenType::KwSay},
    {"end", TokenType::KwEnd},
};

static bool isIdentStart(char c){ return std::isalpha((unsigned char)c) || c=='_'; }
static bool isIdentPart (char c){ return std::isalnum((unsigned char)c) || c=='_'; }

Token Lexer::identifier(char first){
    size_t start = pos-1;
    while (isIdentPart(peek())) advance();
    std::string lex = src.substr(start, pos-start);
    auto it = KW.find(lex);
    if (it!=KW.end()) return make(it->second, lex);
    return make(TokenType::Identifier, lex);
}

Token Lexer::number(char first){
    size_t start = pos-1;
    bool isFloat=false;
    while (std::isdigit(peek())) advance();
    if (peek()=='.'){ isFloat=true; advance(); while (std::isdigit(peek())) advance(); }
    return make(isFloat?TokenType::Float:TokenType::Number, src.substr(start, pos-start));
}

std::vector<Token> Lexer::tokenize(){
    std::vector<Token> out;
    while (pos < src.size()){
        skipWhitespace();
        if (pos>=src.size()) break;
        char c=advance();
        if (isIdentStart(c)) { out.push_back(identifier(c)); continue; }
        if (std::isdigit((unsigned char)c)) { out.push_back(number(c)); continue; }
        if (c=='"'){ out.push_back(string()); continue; }

        switch (c){
            case '(': out.push_back(make(TokenType::LParen,"(")); break;
            case ')': out.push_back(make(TokenType::RParen,")")); break;
            case '{': out.push_back(make(TokenType::LBrace,"{")); break;
            case '}': out.push_back(make(TokenType::RBrace,"}")); break;
            case ':': out.push_back(make(TokenType::Colon,":")); break;
            case ';': out.push_back(make(TokenType::Semicolon,";")); break;
            case ',': out.push_back(make(TokenType::Comma,",")); break;
            case '+': out.push_back(match('=')?make(TokenType::PlusEq,"+="):make(TokenType::Plus,"+")); break;
            case '-': out.push_back(match('=')?make(TokenType::MinusEq,"-="):make(TokenType::Minus,"-")); break;
            case '*': out.push_back(match('=')?make(TokenType::StarEq,"*="):make(TokenType::Star,"*")); break;
            case '/': out.push_back(match('=')?make(TokenType::SlashEq,"/="):make(TokenType::Slash,"/")); break;
            case '%': out.push_back(make(TokenType::Percent,"%")); break;
            case '^': out.push_back(make(TokenType::Caret,"^")); break;
            case '!': out.push_back(match('=')?make(TokenType::BangEq,"!="):make(TokenType::Bang,"!")); break;
            case '&': out.push_back(match('&')?make(TokenType::AndAnd,"&&"):make(TokenType::Amp,"&")); break;
            case '|': out.push_back(match('|')?make(TokenType::OrOr,"||"):make(TokenType::Pipe,"|")); break;
            case '~': out.push_back(make(TokenType::Tilde,"~")); break;
            case '<': out.push_back(match('=')?make(TokenType::LessEq,"<="):make(TokenType::Less,"<")); break;
            case '>': out.push_back(match('=')?make(TokenType::GreaterEq,">="):make(TokenType::Greater,">")); break;
            case '=': out.push_back(match('=')?make(TokenType::EqEq,"=="):make(TokenType::Assign,"=")); break;
            default:  out.push_back(make(TokenType::Unknown,std::string(1,c)));
        }
    }
    out.push_back(make(TokenType::EndOfFile,"<EOF>"));
    return out;
}
