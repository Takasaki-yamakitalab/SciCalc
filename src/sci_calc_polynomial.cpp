#include <Polynomial/monomial_order.hpp>
#include <Polynomial/polynomial.hpp>
#include <Polynomial/factor.hpp>
#include <SciCalc/sci_calc.hpp>
#include <cstdint>
#include <memory>
#include "SciCalc/sci_calc_define.hpp"

namespace polynomial {

template struct polynomial::Polynomial<int, polynomial::LexOrder>;

}

namespace sci_calc {

polynomial::Monomial SciCalc::make_monomial(const NodePtr& node) {
    polynomial::Monomial ret(m_var_id_stack.size());
    if (node->type != ND_OPE || node->id != OP_MUL) return ret;
    for (auto& term : node->children) {
        int id = -1;
        uint64_t n = 0;
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
            if (m_var_id_stack[index] == id) ret.add_exp(index, n);
        }
    }
    return ret;
}

polynomial::Polynomial<int, polynomial::LexOrder> SciCalc::make_polynomial(const NodePtr& node) {
    using Poly = polynomial::Polynomial<int, polynomial::LexOrder>;
    polynomial::Monomial mono(this->m_var_id_stack.size());

    if (node->type != ND_NUM && node->type != ND_OPE && node->type != ND_VAR) return Poly();

    if (node->type == ND_NUM) return Poly(mono, int(node->value.real() + 0.5));
    if (node->type == ND_VAR) {
        for (size_t i = 0; i < m_var_id_stack.size(); i++) {
            if (m_var_id_stack[i] == node->id) {
                mono.set_exp(i, 1);
                return Poly(mono, 1);
            }
        }
        return Poly();
    }
    if (node->type == ND_OPE) {
        if (node->id == OP_ADD) {
            Poly ret;
            for (auto& term : node->children) ret += make_polynomial(term);
            return ret;
        }
        if (node->id == OP_SUB) return make_polynomial(node->children[0]) - make_polynomial(node->children[1]);
        if (node->id == OP_MUL) {
            Poly ret;
            for (auto& term : node->children) ret *= make_polynomial(term);
            return ret;
        }
        if (node->id == OP_DIV)
            return polynomial::pseudoDivide(make_polynomial(node->children[0]), {make_polynomial(node->children[1])}).Q[0];
        if (node->id == OP_POW && node->children[1]->type == ND_NUM) {
            Poly base = make_polynomial(node->children[0]);
            int n = int(node->children[1]->value.real() + 0.5);
            Poly ret(mono, 1);
            for (int i = 0; i < n; i++) {
                ret *= base;
            }
            return ret;
        }
        return Poly();
    }
    return Poly();
}

NodePtr SciCalc::poly2node(const polynomial::Polynomial<int, polynomial::LexOrder>& poly) {
    using Poly = polynomial::Polynomial<int, polynomial::LexOrder>;

    polynomial::Polynomial<int, polynomial::LexOrder> f;
    for (auto it = poly.terms.begin(); it != poly.terms.end(); it++) {
        f.addTerm(it->first, it->second);
    }

    NodePtr ret = std::make_unique<Node>();
    if (f.terms.size() == 0) return std::make_unique<Node>(Complex(0));
    if (f.isConstant()) {
        return std::make_unique<Node>(Complex(f.terms.begin()->second));
    }
    if (f.isMonomial()) {
        ret->type = ND_OPE;
        ret->id = OP_MUL;
        auto& m = f.terms.begin()->first;
        auto& c = f.terms.begin()->second;
        if (c != 1) ret->children.push_back(std::make_unique<Node>(Complex(c)));
        mono2node(ret, m);
        return ret;
    }
    ret->type = ND_OPE;
    ret->id = OP_ADD;
    for (auto it = f.terms.rbegin(); it != f.terms.rend(); it++) {
        ret->children.push_back(poly2node(Poly(it->first, it->second)));
    }
    return ret;
}

void SciCalc::mono2node(NodePtr& dst, const polynomial::Monomial& m) {
    for (size_t i = 0; i < m_var_id_stack.size(); i++) {
        if (m[i] == 0) continue;
        Node var;
        var.type = ND_VAR;
        var.id = m_var_id_stack[i];
        if (m[i] == 1) {
            dst->children.push_back(std::make_unique<Node>(var));
            continue;
        }
        NodePtr nd = std::make_unique<Node>();
        nd->type = ND_OPE;
        nd->id = OP_POW;
        nd->children.push_back(std::make_unique<Node>(var));
        nd->children.push_back(std::make_unique<Node>(Complex((double)m[i])));
        dst->children.push_back(std::move(nd));
    }
}

polynomial::Polynomial<int> SciCalc::expandPoly(NodePtr& node) {
    using Poly = polynomial::Polynomial<int, polynomial::LexOrder>;
    polynomial::Monomial mono(this->m_var_id_stack.size());

    if (node->type == ND_MATH_FUNC || node->type == ND_SPECIAL_FUNC) {
        for (auto& arg : node->children) {
            arg = poly2node(expandPoly(arg));
        }
        return Poly(mono, 0);
    }
    if (node->type == ND_ASSIGN) return Poly(mono, 0);

    if (node->type == ND_NUM) return Poly(mono, int(node->value.real() + 0.5));
    if (node->type == ND_VAR) {
        for (size_t i = 0; i < m_var_id_stack.size(); i++) {
            if (m_var_id_stack[i] == node->id) {
                mono.set_exp(i, 1);
                return Poly(mono, 1);
            }
        }
        return Poly(mono, 0);
    }
    if (node->type == ND_OPE) {
        Poly ret(mono, 0);
        if (node->id == OP_ADD) {
            ret = Poly(mono, 0);
            for (auto& term : node->children) ret += expandPoly(term);
        }
        else if (node->id == OP_SUB) {
            ret = expandPoly(node->children[0]) - expandPoly(node->children[1]);
        }
        else if (node->id == OP_MUL) {
            ret = Poly(mono, 1);
            for (auto& term : node->children) ret *= expandPoly(term);
        }
        else if (node->id == OP_DIV) {
            ret = polynomial::pseudoDivide(expandPoly(node->children[0]), {expandPoly(node->children[1])}).Q[0];
        }
        else if (node->id == OP_POW && node->children[1]->type == ND_NUM) {
            ret = Poly(mono, 1);
            Poly base = expandPoly(node->children[0]);
            int n = int(node->children[1]->value.real() + 0.5);
            for (int i = 0; i < n; i++) {
                ret *= base;
            }
        }
        node = poly2node(ret);
        return ret;
    }
    return Poly(mono, 0);
}

int SciCalc::factorPoly(NodePtr& node) {
    auto f = expandPoly(node);
    auto result = polynomial::factorize(f);
    node = std::make_unique<Node>();
    node->type = ND_OPE;
    node->id = OP_MUL;
    for (auto& fact : result) {
        if (fact.second == 1) {
            node->children.push_back(poly2node(fact.first));
            continue;
        }
        NodePtr nd = std::make_unique<Node>();
        nd->type = ND_OPE;
        nd->id = OP_POW;
        nd->children.push_back(poly2node(fact.first));
        nd->children.push_back(std::make_unique<Node>(Complex((double)fact.second)));
        node->children.push_back(std::move(nd));
    }
    return 0;
}
}  // namespace sci_calc
