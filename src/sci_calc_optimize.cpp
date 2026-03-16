#include "SciCalc/sci_calc.hpp"
#include <cmath>
#include <algorithm>

namespace sci_calc {
    int SciCalc::optimize_node(NodePtr& node) {
        if (node->type == ND_OPE) {
            if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM) return 0;
            // if all children of the operator node are numbers
            double left = node->children[0]->value;
            double right = node->children[1]->value;
            node->type = ND_NUM;
            if (node->id == OP_ADD) node->value = left + right;
            if (node->id == OP_SUB) node->value = left - right;
            if (node->id == OP_MUL) node->value = left * right;
            if (node->id == OP_DIV) node->value = left / right;
            if (node->id == OP_POW) node->value = std::pow(left, right);
            node->children.clear();
            return 0;
        }
        if (node->type == ND_MATH_FUNC) {
            if (node->children[0]->type != ND_NUM) return 0;
            // if the argument of the mathematical function is a number
            double x = node->children[0]->value;
            node->type = ND_NUM;
            node->value = m_functions[node->id].calc(x);
            node->children.clear();
            return 0;
        }
        if (node->type == ND_VAR) {
            for (int i = 0; i < m_var_id_stack.size(); i++) {
                if (m_var_id_stack[i] == node->id) return 0;
            }
            node->type = ND_NUM;
            node->value = m_memory[node->id].value;
            node->children.clear();
            return 0;
        }
        if (node->type == ND_SPECIAL_FUNC) {
            if (m_special_functions[node->id].name == "\\sum") {
                if (node->children[0]->type != ND_NUM || node->children[2]->type != ND_NUM || node->children[3]->type != ND_NUM) return 0;
                // if the children of sum node are numbers
                double start = node->children[2]->value;
                double end = node->children[3]->value;
                node->type = ND_NUM;
                node->value = node->children[0]->value * std::max((int)(end + 0.5) - (int)(start + 0.5) + 1, 0);
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\prod") {
                if (node->children[0]->type != ND_NUM || node->children[2]->type != ND_NUM || node->children[3]->type != ND_NUM) return 0;
                // if the children of prod node are numbers
                double start = node->children[2]->value;
                double end = node->children[3]->value;
                node->type = ND_NUM;
                node->value = std::pow(node->children[0]->value, std::max((int)(end + 0.5) - (int)(start + 0.5) + 1, 0));
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\int") {
                if (node->children[0]->type != ND_NUM || node->children[2]->type != ND_NUM || node->children[3]->type != ND_NUM) return 0;
                // if the children of int node are numbers
                double start = node->children[2]->value;
                double end = node->children[3]->value;
                node->type = ND_NUM;
                node->value = node->children[0]->value * (end - start);
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\diff") {
                if (node->children[0]->type != ND_NUM) return 0;
                // if the body of diff node is a number
                node->type = ND_NUM;
                node->value = 0;
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\perm") {
                if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM) return 0;
                // if the children of perm node are numbers
                double n = node->children[0]->value;
                double r = node->children[1]->value;
                node->type = ND_NUM;
                node->value = permutation(n, r);
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\comb") {
                if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM) return 0;
                // if the children of comb node are numbers
                double n = node->children[0]->value;
                double r = node->children[1]->value;
                node->type = ND_NUM;
                node->value = combination(n, r);
                node->children.clear();
                return 0;
            }
            if (m_special_functions[node->id].name == "\\log") {
                if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM) return 0;
                // if the children of log node are numbers
                double a = node->children[0]->value;
                double x = node->children[1]->value;
                node->type = ND_NUM;
                node->value = std::log(x) / std::log(a);
                node->children.clear();
                return 0;
            }
        }
        return 0;
    }
}