#pragma once
#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    Capsule, Function, Let, Expression, Call, If, Loop, Return, Say
};

struct ASTNode {
    ASTNodeType type;
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::string value;

    ASTNode(ASTNodeType type, const std::string& name = "", const std::string& value = "")
        : type(type), name(name), value(value) {}
};
