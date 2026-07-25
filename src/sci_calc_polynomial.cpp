#include <SciCalc/sci_calc.hpp>

namespace sci_calc {

Monomial SciCalc::make_monomial(NodePtr& node) {
    std::vector<int> ret(m_var_id_stack.size(), 0);
    if (node->type != ND_OPE || node->id != OP_MUL) return ret;
    for (auto& term : node->children) {
        int id = -1;
        int n = 0;
        if (term->type == ND_VAR) {
            id = term->id;
            n = 1;
        } else if (term->type == ND_OPE && term->id == OP_POW && term->children.size() == 2 &&
                   term->children[0]->type == ND_VAR && term->children[1]->type == ND_NUM) {
            id = term->children[0]->id;
            n = (int)(term->children[1]->value.real() + 0.5);
        } else {
            continue;
        }
        for (int index = 0; index < m_var_id_stack.size(); index++) {
            if (m_var_id_stack[index] == id) ret[index] += n;
        }
    }
    return Monomial(ret);
}

template <class Coef, class Order>
Polynomial<Coef, Order> SciCalc::make_polynomial(NodePtr& node) {
    Polynomial<Coef, Order> ret;
    if (node->type != ND_OPE || node->id != OP_ADD) return ret;
    for (auto& term : node->children) {
        Coef coeff(1.0);
        if (term->type == ND_NUM)
            coeff = term->value;
        else if (term->type == ND_OPE && term->id == OP_MUL && term->children[0]->type == ND_NUM)
            coeff = term->children[0]->value;
        Monomial mono = make_monomial(term);
        ret.addTerm(mono, coeff);
    }
    return ret;
}

}  // namespace sci_calc
