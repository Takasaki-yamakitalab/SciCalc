#include <SciCalc/sci_calc.hpp>
#include "SciCalc/sci_calc_define.hpp"

namespace sci_calc {

Monomial SciCalc::make_monomial(NodePtr& node) {
    Monomial ret;
    ret.exp.resize(m_var_id_stack.size(), 0);
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
            if (m_var_id_stack[index] == id) ret.exp[index] += n;
        }
    }
    return ret;
}

Polynomial<Complex> SciCalc::make_polynomial(NodePtr& node) {
    Polynomial<Complex> ret;
    if (node->type != ND_OPE || node->id != OP_ADD) return ret;
    for (auto& term : node->children) {
        Complex coeff = 1.0;
        if (term->type == ND_NUM)
            coeff = term->value;
        else if (term->type == ND_OPE && term->id == OP_MUL && term->children[0]->type == ND_NUM)
            coeff = term->children[0]->value;
        Monomial mono = make_monomial(term);
        ret.terms[mono] += coeff;
    }
    return ret;
}

Monomial mul(const Monomial& a, const Monomial& b) {
    Monomial ret = a;
    for (size_t i = 0; i < ret.exp.size(); i++) {
        ret.exp[i] += b.exp[i];
    }
    return ret;
}
Monomial div(const Monomial& a, const Monomial& b) {
    Monomial ret = a;
    for (size_t i = 0; i < ret.exp.size(); i++) {
        ret.exp[i] -= b.exp[i];
    }
    return ret;
}

Polynomial<Complex> add(const Polynomial<Complex>& a, const Polynomial<Complex>& b) {
    Polynomial<Complex> ret = a;
    for (auto itr = b.terms.begin(); itr != b.terms.end(); itr++) {
        ret.terms[itr->first] += itr->second;
    }
    return ret;
}
Polynomial<Complex> sub(const Polynomial<Complex>& a, const Polynomial<Complex>& b) {
    Polynomial<Complex> ret = a;
    for (auto itr = b.terms.begin(); itr != b.terms.end(); itr++) {
        ret.terms[itr->first] -= itr->second;
    }
    return ret;
}
Polynomial<Complex> mul(const Polynomial<Complex>& a, const Polynomial<Complex>& b) {
    Polynomial<Complex> ret;
    for (auto itr1 = a.terms.begin(); itr1 != a.terms.end(); itr1++) {
        for (auto itr2 = b.terms.begin(); itr2 != b.terms.end(); itr2++) {
            ret.terms[mul(itr1->first, itr2->first)] += itr1->second * itr2->second;
        }
    }
    return ret;
}
Polynomial<Complex> div(const Polynomial<Complex>& a, const Polynomial<Complex>& b);
}  // namespace sci_calc
