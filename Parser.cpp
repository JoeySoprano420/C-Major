#include "Parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : ts(tokens) {}

const Token& Parser::peek() const { return ts[i]; }
const Token& Parser::prev() const { return ts[i-1]; }
bool Parser::check(TokenType t) const { return peek().type==t; }
bool Parser::match(std::initializer_list<TokenType> set){
    for(auto t:set){ if (check(t)){ i++; return true; } }
    return false;
}
const Token& Parser::expect(TokenType t, const char* msg){
    if (!check(t)) throw std::runtime_error(std::string("Parse error: ")+msg);
    return ts[i++];
}

ASTPtr Parser::parseProgram(){ return program(); }

ASTPtr Parser::program(){
    auto root = AST::Node(ASTKind::Program);
    while (!check(TokenType::EndOfFile)){
        if (match({TokenType::KwCapsule})) root->kids.push_back(capsule());
        else if (match({TokenType::KwFunc})) root->kids.push_back(func());
        else throw std::runtime_error("Expected 'capsule' or 'func'");
    }
    return root;
}

ASTPtr Parser::capsule(){
    auto name = expect(TokenType::Identifier, "capsule name").lexeme;
    auto cap = AST::Node(ASTKind::Capsule); cap->name = name;
    expect(TokenType::Colon, "':' after capsule name");
    // capsule body as block-like sequence until 'end'
    while (!match({TokenType::KwEnd})) {
        cap->kids.push_back(statement());
    }
    return cap;
}

ASTPtr Parser::func(){
    auto name = expect(TokenType::Identifier, "func name").lexeme;
    expect(TokenType::LParen, "'(' after func name");
    // simple params: id [, id]*
    auto f = AST::Node(ASTKind::Func); f->name = name;
    if (!check(TokenType::RParen)){
        do {
            auto id = expect(TokenType::Identifier, "param name").lexeme;
            auto p = AST::Node(ASTKind::Param); p->name = id;
            f->kids.push_back(p);
        } while (match({TokenType::Comma}));
    }
    expect(TokenType::RParen, "')' after params");
    // body
    auto b = block();
    f->kids.push_back(b);
    return f;
}

ASTPtr Parser::block(){
    expect(TokenType::Colon, "':' to start block");
    auto b = AST::Node(ASTKind::Block);
    while (!match({TokenType::KwEnd})) {
        b->kids.push_back(statement());
    }
    return b;
}

ASTPtr Parser::statement(){
    if (match({TokenType::KwLet}))     return letDecl();
    if (match({TokenType::KwReturn}))  return returnStmt();
    if (match({TokenType::KwIf}))      return ifStmt();
    if (match({TokenType::KwLoop}))    return loopStmt();
    if (match({TokenType::KwSay}))     return sayStmt();
    // assignment or expression
    return assignOrExprStmt();
}

ASTPtr Parser::letDecl(){
    auto id = expect(TokenType::Identifier,"variable name").lexeme;
    expect(TokenType::Assign,"'=' after variable");
    auto expr = expression();
    expect(TokenType::Semicolon,"';' after declaration");
    auto n = AST::Node(ASTKind::Let); n->name=id; n->kids.push_back(expr);
    return n;
}

ASTPtr Parser::returnStmt(){
    if (check(TokenType::Semicolon)){ i++; auto n=AST::Node(ASTKind::Return); return n; }
    auto e = expression();
    expect(TokenType::Semicolon,"';' after return");
    auto n = AST::Node(ASTKind::Return); n->kids.push_back(e); return n;
}

ASTPtr Parser::ifStmt(){
    expect(TokenType::LParen,"'(' after if");
    auto cond = expression();
    expect(TokenType::RParen,"')' after condition");
    auto thenB = block();
    auto node = AST::Node(ASTKind::If);
    node->kids.push_back(cond);
    node->kids.push_back(thenB);
    if (match({TokenType::KwElse})) {
        auto elseB = block();
        node->kids.push_back(elseB);
    }
    return node;
}

ASTPtr Parser::loopStmt(){
    // loop i from 0 to 10: ... end
    auto id = expect(TokenType::Identifier,"loop variable").lexeme;
    expect(TokenType::KwFrom,"'from'"); auto start = expression();
    expect(TokenType::KwTo,"'to'");     auto stop  = expression();
    auto body = block();
    auto n = AST::Node(ASTKind::Loop); n->name=id;
    n->kids.push_back(start); n->kids.push_back(stop); n->kids.push_back(body);
    return n;
}

ASTPtr Parser::sayStmt(){
    // say "text";
    auto s = expect(TokenType::String,"string after 'say'").lexeme;
    expect(TokenType::Semicolon,"';' after say");
    auto n = AST::Node(ASTKind::Say); n->literal=s; return n;
}

ASTPtr Parser::assignOrExprStmt(){
    // Lookahead for id '=' ...
    if (check(TokenType::Identifier) && ts[i+1].type==TokenType::Assign){
        auto id = expect(TokenType::Identifier,"id").lexeme;
        expect(TokenType::Assign,"=");
        auto e = expression();
        expect(TokenType::Semicolon,"';'");
        auto n = AST::Node(ASTKind::Assign); n->name=id; n->kids.push_back(e);
        return n;
    }
    auto e = expression();
    expect(TokenType::Semicolon,"';' after expression");
    return e; // expression statement node
}

// ---- Expressions (Pratt) ----

int Parser::precedenceOf(const Token& t) const {
    switch (t.type){
        case TokenType::Star: case TokenType::Slash: case TokenType::Percent: return 70;
        case TokenType::Plus: case TokenType::Minus:                           return 60;
        case TokenType::Less: case TokenType::LessEq:
        case TokenType::Greater: case TokenType::GreaterEq:
        case TokenType::EqEq: case TokenType::BangEq:                          return 50;
        case TokenType::AndAnd:                                                return 40;
        case TokenType::OrOr:                                                  return 30;
        default: return -1;
    }
}
bool Parser::isBinaryOp(TokenType t) const {
    switch (t){
        case TokenType::Plus: case TokenType::Minus: case TokenType::Star:
        case TokenType::Slash: case TokenType::Percent:
        case TokenType::Less: case TokenType::LessEq: case TokenType::Greater:
        case TokenType::GreaterEq: case TokenType::EqEq: case TokenType::BangEq:
        case TokenType::AndAnd: case TokenType::OrOr:
            return true;
        default: return false;
    }
}
bool Parser::isUnaryPrefix(TokenType t) const {
    return t==TokenType::Minus || t==TokenType::Bang || t==TokenType::Tilde;
}

ASTPtr Parser::expression(){ return parsePrecedence(0); }

ASTPtr Parser::primary(){
    if (match({TokenType::Number, TokenType::Float, TokenType::String})) {
        return AST::Lit(prev().lexeme);
    }
    if (match({TokenType::Identifier})) {
        return AST::Var(prev().lexeme);
    }
    if (match({TokenType::LParen})) {
        auto e = expression();
        expect(TokenType::RParen, "expected ')'");
        return e;
    }
    throw std::runtime_error("Expected expression");
}

ASTPtr Parser::parsePrecedence(int minPrec){
    ASTPtr lhs;
    if (isUnaryPrefix(peek().type)){
        auto op = peek().lexeme; i++;
        auto rhs = parsePrecedence(80);
        auto n = AST::Node(ASTKind::Unary); n->op=op; n->kids.push_back(rhs);
        lhs = n;
    } else {
        lhs = primary();
    }

    // call postfix
    while (match({TokenType::LParen})){
        // simple arg list
        auto call = AST::Node(ASTKind::Call); call->kids.push_back(lhs);
        if (!check(TokenType::RParen)){
            do { call->kids.push_back(expression()); } while (match({TokenType::Comma}));
        }
        expect(TokenType::RParen, "')' after args");
        lhs = call;
    }

    while (isBinaryOp(peek().type)){
        int prec = precedenceOf(peek());
        if (prec < minPrec) break;
        auto opTok = peek(); i++;
        auto rhs = parsePrecedence(prec+1);
        auto n = AST::Node(ASTKind::Binary); n->op=opTok.lexeme; n->kids.push_back(lhs); n->kids.push_back(rhs);
        lhs = n;
    }
    return lhs;
}
