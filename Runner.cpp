#include "AST.hpp"
#include <iostream>

void execute(const std::shared_ptr<ASTNode>& node) {
    switch (node->type) {
        case ASTNodeType::Capsule:
            for (auto& child : node->children) execute(child);
            break;
        case ASTNodeType::Say:
            std::cout << node->value << std::endl;
            break;
        case ASTNodeType::Let:
            std::cout << "Set " << node->name << " = " << node->value << std::endl;
            break;
        default:
            std::cout << "[Unknown Node]" << std::endl;
    }
}
