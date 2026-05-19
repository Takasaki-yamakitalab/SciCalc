#include <iterator>
#include <memory>
#include "SciCalc/sci_calc.hpp"
#include "SciCalc/sci_calc_define.hpp"

namespace sci_calc {
static int sg_stat = 0;
int SciCalc::expand(NodePtr& node, size_t depth) {
    if (node->type == ND_MATH_FUNC || node->type == ND_SPECIAL_FUNC) {
        for (auto& arg : node->children) {
            expand(arg, depth);
        }
        return 0;
    }
    if (node->type != ND_OPE) return 0;

    optimize_sub(node);
    optimize_div(node);
    optimize_pow(node);

    expand_pow(node);

    if (flatten_add(node)) {
        for (size_t i = 0; i < node->children.size(); i++) {
            expand(node->children[i], depth + 1);
        }
        optimize_flatten_add(node);
        node->hash = node_hash(node.get());
    }

    if (flatten_mul(node)) {
        // std::cout << "DEBUG: flatten mul" << std::endl;
        std::vector<std::vector<NodePtr>> lists;
        for (size_t i = 0; i < node->children.size(); i++) {
            expand(node->children[i], depth + 1);
            std::vector<NodePtr> list;
            if (node->children[i]->type != ND_OPE || node->children[i]->id != OP_ADD) {
                list.push_back(std::move(node->children[i]));
            } else {
                for (size_t j = 0; j < node->children[i]->children.size(); j++) {
                    list.push_back(std::move(node->children[i]->children[j]));
                }
            }
            lists.push_back(std::move(list));
        }

        std::vector<std::vector<NodePtr>> result;
        std::vector<NodePtr> cur;
        cartesian_prod(lists, result, cur, 0);
        node->children.clear();

        node->type = ND_OPE;
        node->id = OP_ADD;
        for (size_t i = 0; i < result.size(); i++) {
            NodePtr term = std::make_unique<Node>();
            term->type = ND_OPE;
            term->id = OP_MUL;
            term->children = std::move(result[i]);
            optimize_flatten_mul(term);
            term->hash = node_hash(term.get());
            node->children.push_back(std::move(term));
        }
        optimize_flatten_add(node);
        node->hash = node_hash(node.get());
    }

    return 0;
}

int SciCalc::flatten_add(NodePtr& node) {
    if (node->type != ND_OPE) return 0;
    if (node->id != OP_ADD && node->id != OP_SUB) return 0;

    std::vector<NodePtr> list;
    if (node->id == OP_ADD) {
        for (auto& child : node->children) {
            if (flatten_add(child) == 0) {
                list.push_back(std::move(child));
                continue;
            }
            list.insert(list.end(),
                        std::make_move_iterator(child->children.begin()),
                        std::make_move_iterator(child->children.end()));
        }
    } else if (node->id == OP_SUB) {
        node->id = OP_ADD;
        if (flatten_add(node->children[0])) {
            list = std::move(node->children[0]->children);
        } else {
            list.push_back(std::move(node->children[0]));
        }
        flatten_add(node->children[1]);
        NodePtr right = std::make_unique<Node>();
        right->type = ND_OPE;
        right->id = OP_MUL;
        right->children.push_back(std::make_unique<Node>(-1.0));
        right->children.push_back(std::move(node->children[1]));
        for (auto& child : right->children) {
            child->hash = node_hash(child.get());
        }
        right->hash = node_hash(right.get());
        list.push_back(std::move(right));
    }

    node->children = std::move(list);
    node->hash = node_hash(node.get());
    return 1;
}

int SciCalc::flatten_mul(NodePtr& node) {
    if (node->type != ND_OPE) return 0;
    if (node->id != OP_MUL && node->id != OP_DIV) return 0;

    std::vector<NodePtr> list;
    if (node->id == OP_MUL) {
        for (auto& child : node->children) {
            if (flatten_mul(child) == 0) {
                list.push_back(std::move(child));
                continue;
            }
            list.insert(list.end(),
                        std::make_move_iterator(child->children.begin()),
                        std::make_move_iterator(child->children.end()));
        }
    } else if (node->id == OP_DIV) {
        node->id = OP_MUL;
        if (flatten_mul(node->children[0])) {
            list = std::move(node->children[0]->children);
        } else {
            list.push_back(std::move(node->children[0]));
        }
        flatten_mul(node->children[1]);
        NodePtr right = std::make_unique<Node>();
        right->type = ND_OPE;
        right->id = OP_POW;
        right->children.push_back(std::move(node->children[1]));
        right->children.push_back(std::make_unique<Node>(-1.0));
        for (auto& child : right->children) {
            child->hash = node_hash(child.get());
        }
        right->hash = node_hash(right.get());
        list.push_back(std::move(right));
    }

    node->children = std::move(list);
    node->hash = node_hash(node.get());
    return 1;
}

int SciCalc::expand_pow(NodePtr& node) {
    if (node->type != ND_OPE || node->id != OP_POW) return 0;
    if (node->children[1]->type == ND_NUM &&
        node->children[1]->value == (size_t)node->children[1]->value) {
        size_t n = node->children[1]->value;
        node->children.erase(node->children.begin() + 1);
        node->type = ND_OPE;
        node->id = OP_MUL;
        for (size_t i = 1; i < n; i++) {
            node->children.push_back(
                std::make_unique<Node>(*node->children[0]));
        }
        node->hash = node_hash(node.get());
    }
    return 0;
}

int SciCalc::cartesian_prod(const std::vector<std::vector<NodePtr>>& sets,
                            std::vector<std::vector<NodePtr>>& dst,
                            std::vector<NodePtr>& cur, size_t depth) {
    if (depth == sets.size()) {
        std::vector<NodePtr> temp;
        for (size_t i = 0; i < cur.size(); i++) {
            temp.push_back(std::make_unique<Node>(*cur[i]));
        }
        dst.push_back(std::move(temp));
        return 0;
    }
    for (size_t i = 0; i < sets[depth].size(); i++) {
        cur.push_back(std::make_unique<Node>(*sets[depth][i]));
        cartesian_prod(sets, dst, cur, depth + 1);
        cur.pop_back();
    }
    return 0;
}
}  // namespace sci_calc
