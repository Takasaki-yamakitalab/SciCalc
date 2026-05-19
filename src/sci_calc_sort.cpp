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

    auto compare = [&](NodePtr& a, NodePtr& b) -> bool {
        if (a->type != b->type) {
            return a->type < b->type;
        }
        if (a->type == ND_VAR) {
            return m_memory[a->id].name < m_memory[b->id].name;
        }
        if (a->type == ND_MATH_FUNC) {
            return m_functions[a->id].name < m_functions[b->id].name;
        }
        return a->hash < b->hash;
    };

    std::sort(node->children.begin(), node->children.end(), compare);
    node->hash = node_hash(node.get());
    return 0;
}
}  // namespace sci_calc
