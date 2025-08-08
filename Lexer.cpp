#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <unordered_map>

enum class TokenType {
    Identifier,
    Keyword,
    Number,
    String,
    Operator,
    Delimiter,
    Comment,
    Whitespace,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& input)
        : input(input), pos(0), line(1), col(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            Token token = nextToken();
            if (token.type == TokenType::EndOfFile)
                break;
            if (token.type != TokenType::Whitespace && token.type != TokenType::Comment)
                tokens.push_back(token);
        }
        return tokens;
    }

private:
    const std::string input;
    size_t pos;
    int line, col;

    const std::unordered_set<std::string> keywords = {
        "if", "else", "while", "for", "return", "break", "continue",
        "int", "float", "char", "string", "void", "bool", "true", "false"
    };

    const std::unordered_set<char> delimiters = {
        '(', ')', '{', '}', '[', ']', ';', ',', '.'
    };

    const std::unordered_set<std::string> operators = {
        "+", "-", "*", "/", "%", "=", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "!", "&", "|"
    };

    char peek(int offset = 0) const {
        if (pos + offset >= input.size()) return '\0';
        return input[pos + offset];
    }

    char get() {
        if (pos >= input.size()) return '\0';
        char c = input[pos++];
        if (c == '\n') { line++; col = 1; }
        else col++;
        return c;
    }

    void unget() {
        if (pos == 0) return;
        pos--;
        if (input[pos] == '\n') line--, col = 1; // Approximate, not perfect
        else col--;
    }

    void skipWhitespace() {
        while (std::isspace(peek())) get();
    }

    Token nextToken() {
        skipWhitespace();
        int startLine = line, startCol = col;
        char c = peek();

        if (c == '\0')
            return {TokenType::EndOfFile, "", line, col};

        // Comments
        if (c == '/' && peek(1) == '/') {
            while (peek() != '\n' && peek() != '\0') get();
            return {TokenType::Comment, "", startLine, startCol};
        }
        if (c == '/' && peek(1) == '*') {
            get(); get(); // consume /*
            while (!(peek() == '*' && peek(1) == '/') && peek() != '\0') get();
            if (peek() == '*' && peek(1) == '/') { get(); get(); }
            return {TokenType::Comment, "", startLine, startCol};
        }

        // Identifiers & Keywords
        if (std::isalpha(c) || c == '_') {
            std::string value;
            while (std::isalnum(peek()) || peek() == '_')
                value += get();
            if (keywords.count(value))
                return {TokenType::Keyword, value, startLine, startCol};
            else
                return {TokenType::Identifier, value, startLine, startCol};
        }

        // Numbers (integer and float)
        if (std::isdigit(c)) {
            std::string value;
            bool isFloat = false;
            while (std::isdigit(peek())) value += get();
            if (peek() == '.') {
                isFloat = true;
                value += get();
                while (std::isdigit(peek())) value += get();
            }
            return {TokenType::Number, value, startLine, startCol};
        }

        // String literals
        if (c == '"') {
            std::string value;
            get(); // consume "
            while (peek() != '"' && peek() != '\0') {
                if (peek() == '\\') { // handle escape
                    value += get();
                    if (peek() != '\0') value += get();
                } else {
                    value += get();
                }
            }
            if (peek() == '"') get();
            return {TokenType::String, value, startLine, startCol};
        }

        // Char literals
        if (c == '\'') {
            std::string value;
            get(); // consume '
            if (peek() == '\\') { value += get(); value += get(); }
            else value += get();
            if (peek() == '\'') get();
            return {TokenType::String, value, startLine, startCol};
        }

        // Operators (longest match)
        for (int len = 2; len >= 1; --len) {
            std::string op;
            for (int i = 0; i < len; ++i) op += peek(i);
            if (operators.count(op)) {
                for (int i = 0; i < len; ++i) get();
                return {TokenType::Operator, op, startLine, startCol};
            }
        }

        // Delimiters
        if (delimiters.count(c)) {
            get();
            return {TokenType::Delimiter, std::string(1, c), startLine, startCol};
        }

        // Whitespace (should be skipped, but for completeness)
        if (std::isspace(c)) {
            get();
            return {TokenType::Whitespace, " ", startLine, startCol};
        }

        // Unknown
        get();
        return {TokenType::Unknown, std::string(1, c), startLine, startCol};
    }
};

// --- Lexer Enhancements ---

namespace LexerFixes {

bool isValidEscape(char c) {
    return c == 'n' || c == 't' || c == '\\' || c == '\'' || c == '\"' || c == 'r' || c == '0';
}

std::string decodeEscape(char c) {
    switch (c) {
        case 'n': return "\n";
        case 't': return "\t";
        case '\\': return "\\";
        case '\'': return "\'";
        case '\"': return "\"";
        case 'r': return "\r";
        case '0': return "\0";
        default: return std::string(1, c);
    }
}

// Patch for unget() to track previous line lengths
class LineTracker {
    std::vector<int> lineLengths;
public:
    void record(int col) { lineLengths.push_back(col); }
    int previous() {
        if (lineLengths.empty()) return 1;
        int len = lineLengths.back();
        lineLengths.pop_back();
        return len;
    }
};

LineTracker tracker;

void enhancedUnget(Lexer& lexer, const std::string& input) {
    if (lexer.pos == 0) return;
    lexer.pos--;
    if (input[lexer.pos] == '\n') {
        lexer.line--;
        lexer.col = tracker.previous();
    } else {
        lexer.col--;
    }
}

// Enhanced string parser
Token parseString(Lexer& lexer) {
    int startLine = lexer.line, startCol = lexer.col;
    std::string value;
    lexer.get(); // consume opening "

    while (lexer.peek() != '\"' && lexer.peek() != '\0') {
        if (lexer.peek() == '\\') {
            lexer.get(); // consume '\'
            char esc = lexer.get();
            if (isValidEscape(esc))
                value += decodeEscape(esc);
            else
                value += esc; // fallback
        } else {
            value += lexer.get();
        }
    }

    if (lexer.peek() == '\"') lexer.get(); // consume closing "
    else return {TokenType::Unknown, value, startLine, startCol}; // unterminated

    return {TokenType::String, value, startLine, startCol};
}

// Enhanced char parser
Token parseChar(Lexer& lexer) {
    int startLine = lexer.line, startCol = lexer.col;
    std::string value;
    lexer.get(); // consume opening '

    if (lexer.peek() == '\\') {
        lexer.get();
        char esc = lexer.get();
        value = decodeEscape(esc);
    } else {
        value += lexer.get();
    }

    if (lexer.peek() == '\'') lexer.get(); // consume closing '
    else return {TokenType::Unknown, value, startLine, startCol}; // unterminated

    return {TokenType::String, value, startLine, startCol};
}

} // namespace LexerFixes
