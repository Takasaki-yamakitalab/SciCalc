#include "SciCalc/sci_calc.hpp"

namespace sci_calc {

    std::string SciCalc::to_string(const NodePtr& node) {
        if (!node) return "";
        
        if (node->type == ND_ASSIGN) {
            std::string left = to_string(node->children[0]);
            std::string right = to_string(node->children[1]);
            return left + ":=" + right;
        }

        if (node->type == ND_OPE) {
            std::string left = to_string(node->children[0]);
            std::string right = to_string(node->children[1]);
            std::string ope;
            switch(node->id) {
                case OP_ADD: ope = "+"; break;
                case OP_SUB: ope = "-"; break;
                case OP_MUL: ope = "*"; break;
                case OP_DIV: ope = "/"; break;
                case OP_POW: ope = "^"; break;
            }
            return "(" + left + ")" + ope + "(" + right + ")";
        }

        if (node->type == ND_NUM) {
            return number_to_string(node);
        }

        if (node->type == ND_SPECIAL_FUNC) {
            return special_func_to_string(node);
        }

        if (node->type == ND_MATH_FUNC) {
            return "@" + m_functions[node->id].name + "(" + to_string(node->children[0]) + ")";
        }

        if (node->type == ND_VAR) {
            return m_memory[node->id].name;
        }

        return "";
    }

    std::string SciCalc::number_to_string(const NodePtr& node) {
        if (!node) return "";
        return std::to_string(node->value);
    }

    std::string SciCalc::special_func_to_string(const NodePtr& node) {
        if (!node) return "";
        return "";  // TODO
    }
}