#include <stdio.h>
#include <cmath>
#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
int SciCalc::is_bracket(char c) {
    if (c == '(' || c == '{') return 1;
    if (c == ')' || c == '}') return -1;
    return 0;
}

std::string SciCalc::rm_brackets_once(std::string& src) {
    if (!is_bracket(src[0]) || !is_bracket(src[src.size() - 1])) return src;
    int brackets = 0;
    for (size_t i = 0; i < src.size(); i++) {
        brackets += is_bracket(src[i]);
        if (brackets == 0 && i != src.size() - 1) return src;
    }
    if (brackets > 0) return src;
    if (brackets < 0) {
        fprintf(stderr, "[ERROR] Improper brackets.\n");
        return std::string();
    }
    src.erase(0, 1);
    src.erase(src.size() - 1, 1);
    return src;
}

std::string SciCalc::rm_brackets(std::string& src) {
    size_t size = src.size();
    while (1) {
        src = rm_brackets_once(src);
        if (size == src.size()) break;
        size = src.size();
    }
    return src;
}

std::string SciCalc::split_by_ope(std::string& formula, const char ope) {
    if (formula.find(ope) == std::string::npos) return std::string();
    int brackets = 0;
    size_t index = 0;
    for (index = formula.size() - 1; index >= 0; index--) {
        if (brackets == 0 && formula[index] == ope) break;
        brackets -= is_bracket(formula[index]);
        if (index == 0) {
            return std::string();
        }
    }
    std::string right(formula.substr(index + 1));
    formula.erase(index);
    return right;
}

std::string SciCalc::split_by_ope(std::string& formula, const char* ope) {
    if (strlen(ope) == 1) return split_by_ope(formula, ope[0]);
    if (formula.find(ope) == std::string::npos) return std::string();
    int brackets = 0;
    size_t index = 0;
    for (index = formula.size() - 1; index >= 0; index--) {
        if (brackets == 0 &&
            strncmp(formula.c_str() + index, ope, strlen(ope)) == 0)
            break;
        brackets -= is_bracket(formula[index]);
        if (index == 0) {
            return std::string();
        }
    }
    std::string right(formula.substr(index + strlen(ope)));
    formula.erase(index);
    return right;
}

size_t SciCalc::node_hash(const Node* node) {
    size_t h = std::hash<int>()(node->type);

    h ^= std::hash<int>()(node->id) + 0x9e3779b9 + (h << 6) + (h >> 2);

    uint64_t bits;
    memcpy(&bits, &node->value, sizeof(double));
    h ^= std::hash<uint64_t>()(bits) + 0x9e3779b9 + (h << 6) + (h >> 2);

    h ^= node->children.size() + 0x9e3779b9 + (h << 6) + (h >> 2);

    for (auto& c : node->children) {
        h ^= c->hash + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

NodePtr SciCalc::parse(std::string& left) {
    this->rm_brackets(left);

    NodePtr node = std::make_unique<Node>();
    std::string right;

    if (left.size() == 0) {
        node->hash = node_hash(node.get());
        return node;
    }

    // try to parse with assignment operators
    if ((right = split_by_ope(left, ":=")).size() != 0) {
        list_var_id(get_memory_id(left));
        node->type = ND_ASSIGN;
        node->children.push_back(parse(left));
        node->children.push_back(parse(right));
        node->hash = node_hash(node.get());
        return node;
    }
    if ((right = split_by_ope(left, "=:")).size() != 0) {
        list_var_id(get_memory_id(right));
        node->type = ND_ASSIGN;
        node->children.push_back(parse(right));
        node->children.push_back(parse(left));
        node->hash = node_hash(node.get());
        return node;
    }

    // try to parse with arithmetic operators
    for (int i = 0; i < OP_END; i++) {
        right = split_by_ope(left, OP_LIST[i]);
        if (right.size() == 0) continue;
        node->type = ND_OPE;
        node->id = i;
        node->children.push_back(parse(left));
        node->children.push_back(parse(right));
        optimize_node(node);
        node->hash = node_hash(node.get());
        return node;
    }

    switch (left[0]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            parse_numbers(left, node);
            optimize_node(node);
            node->hash = node_hash(node.get());
            return node;
            break;
        case '\\':
            // special functions
            parse_special_func_args(left, node);
            optimize_node(node);
            node->hash = node_hash(node.get());
            return node;
            break;
        case '@':
            // mathematical functions
            node->type = ND_MATH_FUNC;
            node->id = detect_func(left);
            rm_func_name(left);
            node->children.push_back(parse(left));
            optimize_node(node);
            node->hash = node_hash(node.get());
            return node;
            break;
        default:
            break;
    }

    // variables
    node->type = ND_VAR;
    node->id = get_memory_id(left);
    if (this->list_all_var) list_var_id(node->id);
    optimize_node(node);
    node->hash = node_hash(node.get());
    return node;
}

int SciCalc::parse_special_func_args(std::string& func, NodePtr& node) {
    node->type = ND_SPECIAL_FUNC;
    node->id = detect_special_func(func);
    rm_special_func_name(func);
    std::vector<std::string> args = get_special_func_args(func);
    if (args.size() != m_special_functions[node->id].arg_num) {
        // the number of arguments is wrong
        node->type = ND_NUM;
        node->value = 0.0;
        node->children.clear();
        return -1;
    }
    if (m_special_functions[node->id].name == "\\sum" ||
        m_special_functions[node->id].name == "\\prod" ||
        m_special_functions[node->id].name == "\\int" ||
        m_special_functions[node->id].name == "\\diff") {
        list_var_id(get_memory_id(args[1]));
    }
    for (int i = 0; i < args.size(); i++) {
        node->children.push_back(parse(args[i]));
    }
    return 0;
}

int SciCalc::parse_numbers(std::string& formula, NodePtr& node) {
    node->type = ND_NUM;
    node->value = std::atof(formula.c_str());
    int index = 0;
    int period = 0;
    for (index = 0; index < formula.size(); index++) {
        if (formula[index] == '.') {
            if (period) break;
            period++;
            continue;
        }
        if (formula[index] == '0' || formula[index] == '1' ||
            formula[index] == '2' || formula[index] == '3' ||
            formula[index] == '4' || formula[index] == '5' ||
            formula[index] == '6' || formula[index] == '7' ||
            formula[index] == '8' || formula[index] == '9') {
            continue;
        }
        break;
    }
    if (index == formula.size()) {
        node->type = ND_NUM;
        node->value = std::atof(formula.c_str());
        return 0;
    }
    std::string right(formula.substr(index));
    formula.erase(index);
    node->type = ND_OPE;
    node->id = OP_MUL;
    node->children.push_back(parse(formula));
    node->children.push_back(parse(right));
    return 0;
}

Complex SciCalc::evaluate_node(const NodePtr& node) {
    if (node == NULL) return 0.0;
    if (node->type == ND_NUM) return node->value;
    if (node->type == ND_VAR) return m_memory[node->id].value;
    if (node->type == ND_MATH_FUNC)
        return m_functions[node->id].calc(evaluate_node(node->children[0]));
    if (node->type == ND_SPECIAL_FUNC)
        return m_special_functions[node->id].calc(node->children);
    if (node->type == ND_ASSIGN) {
        if (node->children[0]->type != ND_VAR || node->children.size() != 2) {
            fprintf(stderr, "Failed to assign.\n");
            return NAN;
        }
        return m_memory[node->children[0]->id].value =
                   evaluate_node(node->children[1]);
    }
    if (node->type == ND_OPE) {
        if (node->id == OP_ADD)
            return evaluate_node(node->children[0]) +
                   evaluate_node(node->children[1]);
        if (node->id == OP_SUB)
            return evaluate_node(node->children[0]) -
                   evaluate_node(node->children[1]);
        if (node->id == OP_MUL)
            return evaluate_node(node->children[0]) *
                   evaluate_node(node->children[1]);
        if (node->id == OP_DIV)
            return evaluate_node(node->children[0]) /
                   evaluate_node(node->children[1]);
        if (node->id == OP_POW)
            return std::pow(evaluate_node(node->children[0]),
                            evaluate_node(node->children[1]));
    }
    return 0.0;
}
}  // namespace sci_calc
