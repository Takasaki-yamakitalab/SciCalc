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
        std::string ope;
        switch (node->id) {
            case OP_ADD:
                ope = "+";
                break;
            case OP_SUB:
                printf("sub!\n");
                ope = "-";
                break;
            case OP_MUL:
                ope = "*";
                break;
            case OP_DIV:
                ope = "/";
                break;
            case OP_POW:
                ope = "^";
                break;
        }
        std::string ret = "";
        for (size_t i = 0; i < node->children.size(); i++) {
            std::string term = to_string(node->children[i]);
            if (need_paren(node, node->children[i])) term = "(" + term + ")";
            if (!term.empty() && !ret.empty() && !(ope == "+" && term[0] == '-')) ret += ope;
            ret += term;
        }
        return ret;
    }

    if (node->type == ND_NUM) {
        return number_to_string(node);
    }

    if (node->type == ND_SPECIAL_FUNC) {
        return special_func_to_string(node);
    }

    if (node->type == ND_MATH_FUNC) {
        return m_functions[node->id].name + "(" + to_string(node->children[0]) + ")";
    }

    if (node->type == ND_VAR) {
        return m_memory[node->id].name;
    }

    return "";
}

std::string SciCalc::number_to_string(const NodePtr& node) {
    if (!node) return "";
    return to_string_g(node->value);
}

std::string SciCalc::special_func_to_string(const NodePtr& node) {
    if (!node) return "";
    std::string ret = "";
    ret += m_special_functions[node->id].name;
    for (auto& arg : node->children) {
        ret += "{" + to_string(arg) + "}";
    }
    return ret;
}

bool SciCalc::need_paren(const NodePtr& node, const NodePtr& child) {
    if (child->type == ND_NUM && node->children[0] == child) return false;
    if (child->type == ND_NUM && child->value < 0) {
        if (node->type == ND_OPE && node->id == OP_ADD) return false;
        return true;
    }
    if (child->type != ND_OPE) return false;
    if (node->type == ND_OPE) {
        if (node->id == OP_ADD) return false;
        if (node->id == OP_SUB) {
            if (child->id != OP_ADD) return false;
            return true;
        }
        if (node->id == OP_MUL) {
            if (child->id == OP_ADD || child->id == OP_SUB) return true;
            return false;
        }
        if (node->id == OP_DIV) {
            if (child->id == OP_ADD || child->id == OP_SUB || child->id == OP_MUL) return true;
            return false;
        }
        if (node->id == OP_POW) {
            return true;
        }
    }

    return false;
}

std::string SciCalc::to_string_g(const double value, const int precision) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*g", precision, value);
    return std::string(buf);
}
}  // namespace sci_calc
