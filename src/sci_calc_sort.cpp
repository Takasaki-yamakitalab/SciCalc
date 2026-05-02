#include "SciCalc/sci_calc.hpp"
#include <algorithm>

namespace sci_calc {
    int SciCalc::sort_children(NodePtr& node) {
        if (node->type != ND_OPE) return 0;
        if (node->id == OP_SUB || node->id == OP_DIV || node->id == OP_POW) return 0;
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
        return 0;
    }
}