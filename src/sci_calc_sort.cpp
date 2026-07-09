#include "SciCalc/sci_calc.hpp"
#include <algorithm>

namespace sci_calc {
int SciCalc::sort_children(NodePtr& node) {
    if (node->type != ND_OPE) return 0;
    if (node->id == OP_ADD) return sort_flatten_add(node);
    if (node->id == OP_MUL) return sort_flatten_mul(node);
    return 0;
}

int SciCalc::sort_flatten_add(NodePtr& node) {
    if (node->type != ND_OPE) return 0;
    if (node->id != OP_ADD) return 0;

    return 0;
}

int SciCalc::sort_flatten_mul(NodePtr& node) {
    if (node->type != ND_OPE) return 0;
    if (node->id != OP_MUL) return 0;

    auto comp = [&](auto self, NodePtr& a, NodePtr& b) -> bool {
        // number must be first.
        if (a->type == ND_NUM) return false;
        if (b->type == ND_NUM) return false;

        if (a->type == ND_VAR && b->type == ND_VAR)
            return m_memory[a->id].name < m_memory[b->id].name;

        if (a->type == ND_OPE && a->id == OP_POW)
            return self(self, a->children[0], b);
        if (b->type == ND_OPE && b->id == OP_POW)
            return self(self, a, b->children[0]);

        return a->hash < b->hash;
    };

    auto compare = [&](NodePtr& a, NodePtr& b) -> bool {
        return comp(comp, a, b);
    };

    std::sort(node->children.begin(), node->children.end(), compare);
    node->hash = node_hash(node.get());
    return 0;
}
}  // namespace sci_calc
