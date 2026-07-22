#include "SciCalc/sci_calc.hpp"
#include <cmath>
#include <algorithm>

namespace sci_calc {
bool SciCalc::is_equal(const Node* a, const Node* b) {
    if (a->hash != b->hash) return false;
    if (a->type != b->type) return false;
    if (a->id != b->id) return false;
    if (a->value != b->value) return false;

    if (a->children.size() != b->children.size()) return false;
    for (size_t i = 0; i < a->children.size(); i++) {
        if (!is_equal(a->children[i].get(), b->children[i].get())) return false;
    }

    return true;
}

uint32_t SciCalc::change_optimizer(const std::string& opt) {
    if (opt == "none" || opt == "None" || opt == "NONE") {
        this->m_optimizer = OPTIMIZER_NONE;
    }
    if (opt == "default" || opt == "Default" || opt == "DEFAULT") {
        this->m_optimizer = OPTIMIZER_DEFAULT;
    }
    if (opt == "all" || opt == "All" || opt == "ALL") {
        this->m_optimizer = OPTIMIZER_ALL;
    }
    return this->m_optimizer;
}

uint32_t SciCalc::add_optimizer(const std::string& opt) {
    uint32_t o = OPTIMIZER_NONE;
    if (opt == "constant folding" || opt == "Constant Folding" || opt == "cf" ||
        opt == "CF" || opt == "CONSTANT FOLDING") {
        o = OPTIMIZER_CONST_FOLDING;
    }
    if (opt == "constant propagation" || opt == "Constant Propagation" ||
        opt == "cp" || opt == "CP" || opt == "CONSTANT PROPAGATION") {
        o = OPTIMIZER_CONST_PROPAGATION;
    }
    if (opt == "algebraic simplification" ||
        opt == "Algebraic Simplification" || opt == "as" || opt == "AS" ||
        opt == "ALGEBRAIC SIMPLIFICATION") {
        o = OPTIMIZER_ALGEBRAIC_SIMPLIFICATION;
    }
    if (opt == "combine like terms" || opt == "Combine Like Terms" ||
        opt == "clt" || opt == "CLT" || opt == "COMBINE LIKE TERMS") {
        o = OPTIMIZER_COMBINE_LIKE_TERMS;
    }
    this->m_optimizer |= o;
    return o;
}

int SciCalc::optimize_node(NodePtr& node) {
    if (this->m_optimizer == OPTIMIZER_NONE) return 0;
    if (this->m_optimizer & OPTIMIZER_CONST_FOLDING)
        optimizer_const_folding(node);
    if (this->m_optimizer & OPTIMIZER_CONST_PROPAGATION)
        optimizer_const_propagation(node);
    if (this->m_optimizer & OPTIMIZER_ALGEBRAIC_SIMPLIFICATION)
        optimizer_algebraic_simplification(node);
    if (this->m_optimizer & OPTIMIZER_COMBINE_LIKE_TERMS)
        optimizer_combine_like_terms(node);
    return 0;
}

int SciCalc::optimizer_const_folding(NodePtr& node) {
    if (node->type == ND_OPE) {
        if (node->children[0]->type != ND_NUM ||
            node->children[1]->type != ND_NUM)
            return 0;
        // if all children of the operator node are numbers
        Complex left = node->children[0]->value;
        Complex right = node->children[1]->value;
        node->type = ND_NUM;
        if (node->id == OP_ADD) node->value = left + right;
        if (node->id == OP_SUB) node->value = left - right;
        if (node->id == OP_MUL) node->value = left * right;
        if (node->id == OP_DIV) node->value = left / right;
        if (node->id == OP_POW) node->value = std::pow(left, right);
        node->id = 0;
        node->children.clear();
        return 0;
    }
    if (node->type == ND_MATH_FUNC) {
        if (node->children[0]->type != ND_NUM) return 0;
        // if the argument of the mathematical function is a number
        Complex x = node->children[0]->value;
        node->type = ND_NUM;
        node->value = m_functions[node->id].calc(x);
        node->id = 0;
        node->children.clear();
        return 0;
    }
    if (node->type == ND_SPECIAL_FUNC) {
        if (m_special_functions[node->id].name == "\\sum") {
            if (node->children[0]->type != ND_NUM ||
                node->children[2]->type != ND_NUM ||
                node->children[3]->type != ND_NUM)
                return 0;
            // if the children of sum node are numbers
            double start = node->children[2]->value.real();
            double end = node->children[3]->value.real();
            node->type = ND_NUM;
            node->value = node->children[0]->value * (double)std::max((int)(end + 0.5) - (int)(start + 0.5) + 1, 0);
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\prod") {
            if (node->children[0]->type != ND_NUM ||
                node->children[2]->type != ND_NUM ||
                node->children[3]->type != ND_NUM)
                return 0;
            // if the children of prod node are numbers
            double start = node->children[2]->value.real();
            double end = node->children[3]->value.real();
            node->type = ND_NUM;
            node->value = std::pow(
                node->children[0]->value,
                std::max((int)(end + 0.5) - (int)(start + 0.5) + 1, 0));
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\int") {
            if (node->children[0]->type != ND_NUM ||
                node->children[2]->type != ND_NUM ||
                node->children[3]->type != ND_NUM)
                return 0;
            // if the children of int node are numbers
            double start = node->children[2]->value.real();
            double end = node->children[3]->value.real();
            node->type = ND_NUM;
            node->value = node->children[0]->value * (end - start);
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\diff") {
            if (node->children[0]->type != ND_NUM) return 0;
            // if the body of diff node is a number
            node->type = ND_NUM;
            node->value = 0;
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\perm") {
            if (node->children[0]->type != ND_NUM ||
                node->children[1]->type != ND_NUM)
                return 0;
            // if the children of perm node are numbers
            double n = node->children[0]->value.real();
            double r = node->children[1]->value.real();
            node->type = ND_NUM;
            node->value = permutation(n, r);
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\comb") {
            if (node->children[0]->type != ND_NUM ||
                node->children[1]->type != ND_NUM)
                return 0;
            // if the children of comb node are numbers
            double n = node->children[0]->value.real();
            double r = node->children[1]->value.real();
            node->type = ND_NUM;
            node->value = combination(n, r);
            node->id = 0;
            node->children.clear();
            return 0;
        }
        if (m_special_functions[node->id].name == "\\log") {
            if (node->children[0]->type != ND_NUM ||
                node->children[1]->type != ND_NUM)
                return 0;
            // if the children of log node are numbers
            double a = node->children[0]->value.real();
            double x = node->children[1]->value.real();
            node->type = ND_NUM;
            node->value = std::log(x) / std::log(a);
            node->id = 0;
            node->children.clear();
            return 0;
        }
    }
    return 0;
}

int SciCalc::optimizer_const_propagation(NodePtr& node) {
    if (node->type != ND_VAR) return 0;

    for (int i = 0; i < m_var_id_stack.size(); i++) {
        if (m_var_id_stack[i] == node->id) return 0;
    }
    node->type = ND_NUM;
    node->value = m_memory[node->id].value;
    node->id = 0;
    node->children.clear();
    return 0;
}

int SciCalc::optimizer_algebraic_simplification(NodePtr& node) {
    if (node->type != ND_OPE) return 0;

    if (node->id == OP_ADD) {
        if (node->children[0]->type == ND_NUM &&
            node->children[0]->value == 0.0) {
            node = std::move(node->children[1]);
            return 0;
        }
        if (node->children[1]->type == ND_NUM &&
            node->children[1]->value == 0.0) {
            node = std::move(node->children[0]);
            return 0;
        }
    }
    if (node->id == OP_SUB) {
        if (node->children[1]->type == ND_NUM &&
            node->children[1]->value == 0.0) {
            node = std::move(node->children[0]);
            return 0;
        }
    }
    if (node->id == OP_MUL) {
        if (node->children[0]->type == ND_NUM &&
                node->children[0]->value == 0.0 ||
            node->children[1]->type == ND_NUM &&
                node->children[1]->value == 0.0) {
            node->type = ND_NUM;
            node->id = 0;
            node->value = 0;
            node->children.clear();
            return 0;
        }
        if (node->children[0]->type == ND_NUM &&
            node->children[0]->value == 1.0) {
            node = std::move(node->children[1]);
            return 0;
        }
        if (node->children[1]->type == ND_NUM &&
            node->children[1]->value == 1.0) {
            node = std::move(node->children[0]);
            return 0;
        }
    }
    if (node->id == OP_DIV) {
        if (node->children[0]->type == ND_NUM &&
            node->children[0]->value == 0.0) {
            node->type = ND_NUM;
            node->id = 0;
            node->value = 0;
            node->children.clear();
            return 0;
        }
        if (node->children[1]->type == ND_NUM &&
            node->children[1]->value == 1.0) {
            node = std::move(node->children[0]);
            return 0;
        }
    }
    return 0;
}

int SciCalc::optimizer_combine_like_terms(NodePtr& node) {
    if (node->type != ND_OPE) return 0;

    if (node->id == OP_ADD || node->id == OP_MUL) {
        if (node->children[0]->hash > node->children[1]->hash) {
            std::swap(node->children[0], node->children[1]);
            return 0;
        }
    }

    if (!is_equal(node->children[0].get(), node->children[1].get())) return 0;

    if (node->id == OP_ADD) {
        node->id = OP_MUL;
        node->children[0]->type = ND_NUM;
        node->children[0]->id = 0;
        node->children[0]->value = 2;
        node->children[0]->children.clear();
        return 0;
    }
    if (node->id == OP_SUB) {
        node->type = ND_NUM;
        node->id = 0;
        node->value = 0;
        node->children.clear();
        return 0;
    }
    if (node->id == OP_MUL) {
        node->id = OP_POW;
        node->children[1]->type = ND_NUM;
        node->children[1]->id = 0;
        node->children[1]->value = 2;
        node->children[1]->children.clear();
        return 0;
    }

    return 0;
}

int SciCalc::optimize_flatten_add(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_ADD) return 0;
    Complex value = 0;
    for (auto it = node->children.begin(); it != node->children.end(); it++) {
        if ((*it)->type == ND_NUM) continue;
        Complex coef = 1.0;
        if ((*it)->type == ND_OPE && (*it)->id == OP_MUL &&
            (*it)->children[0]->type == ND_NUM) {
            coef = (*it)->children[0]->value;
            (*it)->children.erase((*it)->children.begin());
            if ((*it)->children.size() == 1) {
                *it = std::move((*it)->children[0]);
            }
            (*it)->hash = node_hash((*it).get());
        }
        for (auto i = it; i != node->children.end();) {
            if (it == i) {
                i++;
                continue;
            }
            if ((*i)->type == ND_OPE && (*i)->id == OP_MUL &&
                (*i)->children[0]->type == ND_NUM) {
                NodePtr temp = std::make_unique<Node>(**i);
                temp->children.erase(temp->children.begin());
                if (temp->children.size() == 1) {
                    temp = std::move(temp->children[0]);
                }
                temp->hash = node_hash(temp.get());
                if (is_equal((*it).get(), temp.get())) {
                    coef += (*i)->children[0]->value;
                    i = node->children.erase(i);
                    continue;
                }
            } else if (is_equal((*it).get(), (*i).get())) {
                coef += 1.0;
                i = node->children.erase(i);
                continue;
            }

            i++;
        }
        if (coef != 1.0) {
            NodePtr nd = std::make_unique<Node>();
            nd->type = ND_OPE;
            nd->id = OP_MUL;
            NodePtr n = std::make_unique<Node>(coef);
            n->hash = node_hash(n.get());
            nd->children.push_back(std::move(n));
            nd->children.push_back(std::move(*it));
            flatten_mul(nd);
            optimize_flatten_mul(nd);
            nd->hash = node_hash(nd.get());
            *it = std::move(nd);
        }
    }
    for (auto it = node->children.begin(); it != node->children.end();) {
        if ((*it)->type == ND_NUM) {
            value += (*it)->value;
            it = node->children.erase(it);
        } else
            it++;
    }
    sort_children(node);
    if (node->children.size() == 0 || value != 0.0) {
        NodePtr nd = std::make_unique<Node>(value);
        nd->hash = node_hash(nd.get());
        if (node->children.size())
            node->children.push_back(std::move(nd));
        else
            node = std::move(nd);
    }
    while (node->type == ND_OPE && node->children.size() == 1) {
        node = std::move(node->children[0]);
    }
    node->hash = node_hash(node.get());
    return 0;
}

int SciCalc::optimize_flatten_mul(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_MUL) return 0;
    Complex coef = 1.0;

    // y*2*x*y*3*x^2*x*x^(-1) -> 6, y*x*x^2*x*x^(-1)
    for (auto it = node->children.begin(); it != node->children.end();) {
        if ((*it)->type == ND_NUM) {
            coef *= (*it)->value;
            it = node->children.erase(it);
        } else
            it++;
    }
    if (coef == 0.0) {
        node = std::make_unique<Node>(0);
        node->hash = node_hash(node.get());
        return 0;
    }

    // 6, y*x*x^2*x*x^(-1) -> 6, y^1*x^2*x^2*x^(-1)
    for (auto it = node->children.begin(); it != node->children.end(); it++) {
        double index = 1;
        for (auto i = it + 1; i != node->children.end();) {
            if (is_equal((*it).get(), (*i).get())) {
                index++;
                i = node->children.erase(i);
            } else
                i++;
        }
        NodePtr nd = std::make_unique<Node>();
        nd->type = ND_OPE;
        nd->id = OP_POW;
        NodePtr n = std::make_unique<Node>(index);
        n->hash = node_hash(n.get());
        nd->children.push_back(std::move(*it));
        nd->children.push_back(std::move(n));
        optimize_pow(nd);
        nd->hash = node_hash(nd.get());
        *it = std::move(nd);
    }

    // 6, y^1*x^2*x^2*x^(-1) -> 6, y*x^3
    for (auto it = node->children.begin(); it != node->children.end(); it++) {
        for (auto i = it + 1; i != node->children.end();) {
            if (is_equal((*it)->children[0].get(), (*i)->children[0].get())) {
                NodePtr n = std::make_unique<Node>();
                n->type = ND_OPE;
                n->id = OP_ADD;
                n->children.push_back(std::move((*it)->children[1]));
                n->children.push_back(std::move((*i)->children[1]));
                n->hash = node_hash(n.get());
                (*it)->children[1] = std::move(n);
                i = node->children.erase(i);
            } else
                i++;
        }
        if (flatten_add((*it)->children[1]))
            optimize_flatten_add((*it)->children[1]);
        if ((*it)->children[1]->type == ND_NUM && (*it)->children[1]->value == 1.0) {
            (*it) = std::move((*it)->children[0]);
        } else if ((*it)->children[1]->type == ND_NUM && (*it)->children[1]->value == 0.0) {
            (*it) = std::make_unique<Node>(1.0);
        }
        (*it)->hash = node_hash((*it).get());
    }

    // 6, y*x^3 -> 6, x^3*y
    sort_children(node);

    // 6, x^3*y -> 6*x^3*y
    if (node->children.size() == 0 || coef != 1.0) {
        NodePtr nd = std::make_unique<Node>(coef);
        nd->hash = node_hash(nd.get());
        if (node->children.size())
            node->children.insert(node->children.begin(), std::move(nd));
        else
            node = std::move(nd);
    }

    while (node->type == ND_OPE && node->children.size() == 1) {
        node = std::move(node->children[0]);
    }
    node->hash = node_hash(node.get());
    return 0;
}

int SciCalc::optimize_sub(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_SUB) return 0;
    if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM)
        return 0;
    Complex value = node->children[0]->value - node->children[1]->value;
    node->type = ND_NUM;
    node->id = 0;
    node->value = value;
    node->children.clear();
    node->hash = node_hash(node.get());
    return 0;
}
int SciCalc::optimize_div(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_DIV) return 0;
    if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM)
        return 0;
    Complex value = node->children[0]->value / node->children[1]->value;
    node->type = ND_NUM;
    node->id = 0;
    node->value = value;
    node->children.clear();
    node->hash = node_hash(node.get());
    return 0;
}
int SciCalc::optimize_pow(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_POW) return 0;
    if (node->children[0]->type == ND_OPE && node->children[0]->id == OP_POW) {
        // (a^b)^c -> a^(b*c)
        NodePtr base = std::make_unique<Node>(*node->children[0]->children[0]);
        NodePtr index = std::make_unique<Node>();
        index->type = ND_OPE;
        index->id = OP_MUL;
        index->children.push_back(std::move(node->children[0]->children[1]));
        index->children.push_back(std::move(node->children[1]));
        flatten_mul(index);
        optimize_flatten_mul(index);
        index->hash = node_hash(index.get());
        node->children.clear();
        node->children.push_back(std::move(base));
        node->children.push_back(std::move(index));
        node->hash = node_hash(node.get());
        return 0;
    }
    if (node->children[0]->type != ND_NUM || node->children[1]->type != ND_NUM)
        return 0;
    // ex. 2^2 -> 4
    Complex value = std::pow(node->children[0]->value, node->children[1]->value);
    node->type = ND_NUM;
    node->id = 0;
    node->value = value;
    node->children.clear();
    node->hash = node_hash(node.get());
    return 0;
}
}  // namespace sci_calc
