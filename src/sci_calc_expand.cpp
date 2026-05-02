#include "SciCalc/sci_calc.hpp"
#include <iostream>

namespace sci_calc {
    static int sg_stat = 0;
    int SciCalc::expand(NodePtr& node, size_t depth) {
        if (node->type != ND_OPE) return 0;
        // std::cout << "DEBUG: expand [" << depth << "] " << to_string(node) << std::endl;

        optimize_sub(node);
        optimize_div(node);
        optimize_pow(node);

        expand_pow(node);

        if (flatten_add(node)) {
            // std::cout << "DEBUG: flatten add" << std::endl;
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
                    list.push_back(std::make_unique<Node>(*node->children[i]));
                }
                else {
                    for (size_t j = 0; j < node->children[i]->children.size(); j++) {
                        list.push_back(std::make_unique<Node>(*node->children[i]->children[j]));
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

        // std::cout << "DEBUG: complete [" << depth << "] " << to_string(node) << std::endl;
        return 0;
    }

    int SciCalc::flatten_add(NodePtr& node) {
        if (node->type != ND_OPE) return 0;
        if (node->id != OP_ADD) return 0;

        if (flatten_add(node->children[0])) {
            std::vector<NodePtr> list = std::move(node->children[0]->children);
            list.push_back(std::move(node->children[1]));
            node->children = std::move(list);
        }
        if (flatten_add(node->children[1])) {
            std::vector<NodePtr> list = std::move(node->children[1]->children);
            list.insert(list.begin(), std::move(node->children[0]));
            node->children = std::move(list);
        }

        node->hash = node_hash(node.get());
        return 1;
    }

    int SciCalc::flatten_mul(NodePtr& node) {
        if (node->type != ND_OPE) return 0;
        if (node->id != OP_MUL) return 0;

        if (flatten_mul(node->children[0])) {
            std::vector<NodePtr> list = std::move(node->children[0]->children);
            list.push_back(std::move(node->children[1]));
            node->children = std::move(list);
        }
        if (flatten_mul(node->children[1])) {
            std::vector<NodePtr> list = std::move(node->children[1]->children);
            list.insert(list.begin(), std::move(node->children[0]));
            node->children = std::move(list);
        }

        node->hash = node_hash(node.get());
        return 1;
    }

    int SciCalc::expand_pow(NodePtr& node) {
        if (node->type != ND_OPE || node->id != OP_POW) return 0;
        if (node->children[1]->type == ND_NUM && node->children[1]->value == (size_t)node->children[1]->value) {
            size_t n = node->children[1]->value;
            node->children.erase(node->children.begin() + 1);
            node->type = ND_OPE;
            node->id = OP_MUL;
            for (size_t i = 1; i < n; i++) {
                node->children.push_back(std::make_unique<Node>(*node->children[0]));
            }
            node->hash = node_hash(node.get());
        }
        return 0;
    }

    int SciCalc::cartesian_prod(const std::vector<std::vector<NodePtr>>& sets, std::vector<std::vector<NodePtr>>& dst, std::vector<NodePtr>& cur, size_t depth) {
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
}