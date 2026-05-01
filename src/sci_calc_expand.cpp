#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
    int SciCalc::expand(NodePtr& node) {
        if (node->type != ND_OPE) return 0;

        if (flatten_add(node)) {
            for (size_t i = 0; i < node->children.size(); i++) {
                expand(node->children[i]);
            }
            node->hash = node_hash(node.get());
            return 0;
        }

        if (flatten_mul(node)) {
            std::vector<std::vector<NodePtr>> lists;
            for (size_t i = 0; i < node->children.size(); i++) {
                expand(node->children[i]);
                if (node->children[i]->type != ND_OPE || node->children[i]->type != OP_ADD) {
                    lists.push_back({ std::make_unique<Node>(*node->children[i]) });
                    continue;
                }
                std::vector<NodePtr> list;
                for (size_t j = 0; j < node->children[i]->children.size(); j++) {
                    list.push_back(std::make_unique<Node>(*node->children[i]->children[j]));
                }
                lists.push_back(std::move(list));
            }

            std::vector<NodePtr> result = { std::make_unique<Node>() };
            result[0]->type = ND_NUM;
            result[0]->value = 1.0;
            result[0]->hash = node_hash(result[0].get());
            for (size_t i = 0; i < lists.size(); i++) {
                std::vector<NodePtr> next;
                for (size_t j = 0; j < lists[i].size(); j++) {
                    for (size_t k = 0; k < result.size(); k++) {
                        NodePtr temp = std::make_unique<Node>();
                        temp->type = ND_OPE;
                        temp->id = OP_MUL;
                        temp->children.push_back(std::make_unique<Node>(*result[k]));
                        temp->children.push_back(std::make_unique<Node>(*lists[i][j]));
                        temp->hash = node_hash(temp.get());
                        next.push_back(std::move(temp));
                    }
                }
                result = std::move(next);
            }

            node->id = OP_ADD;
            node->children = std::move(result);
            node->hash = node_hash(node.get());
            return 0;
        }
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

    int SciCalc::fold_add(NodePtr& node) {
        if (node->type != ND_OPE) return 0;
        if (node->id != OP_ADD) return 0;
        if (node->children.size() <= 2) return 0;

        std::vector<NodePtr> list = std::move(node->children);
        node->children.clear();

        while (list.size() > 2) {
            NodePtr nd = std::make_unique<Node>();
            nd->type = ND_OPE;
            nd->id = OP_ADD;
            nd->children.push_back(std::move(list[0]));
            nd->children.push_back(std::move(list[1]));
            nd->hash = node_hash(nd.get());
            list[0] = std::move(nd);
            list.erase(list.begin() + 1);
        }
        node->children = std::move(list);
        node->hash = node_hash(node.get());
        return 1;
    }
}