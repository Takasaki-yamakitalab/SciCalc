#include <SciCalc/sci_calc.hpp>
#include "SciCalc/polynomial.hpp"

namespace sci_calc {

bool LexOrder::operator()(const Monomial& a, const Monomial& b) const {
    for (size_t i = 0; i < a.size(); i++) {
        if (a.exp(i) != b.exp(i)) return a.exp(i) < b.exp(i);
    }
    return false;
}
bool GrLexOrder::operator()(const Monomial& a, const Monomial& b) const {
    auto da = a.degree();
    auto db = b.degree();
    if (da != db) return da < db;
    for (size_t i = 0; i < a.size(); i++) {
        if (a.exp(i) != b.exp(i)) return a.exp(i) < b.exp(i);
    }
    return false;
}
bool GrevLexOrder::operator()(const Monomial& a, const Monomial& b) const {
    auto da = a.degree();
    auto db = b.degree();
    if (da != db) return da < db;
    for (int i = (int)a.size() - 1; i >= 0; i--) {
        if (a.exp(i) != b.exp(i)) return a.exp(i) > b.exp(i);
    }
    return false;
}

template <class Coef, class Order>
Term<Coef> Polynomial<Coef, Order>::leadingTerm() const {
    assert(!terms.empty());
    auto it = std::prev(terms.end());
    if (std::is_same<Order, GrevLexOrder>::value) it = terms.begin();
    return {it->first, it->second};
}
template <class Coef, class Order>
const Monomial& Polynomial<Coef, Order>::leadingMonomial() const {
    assert(!terms.empty());
    if (std::is_same<Order, GrevLexOrder>::value) return terms.begin()->first;
    return std::prev(terms.end())->first;
}
template <class Coef, class Order>
const Coef& Polynomial<Coef, Order>::leadingCoefficient() const {
    assert(!terms.empty());
    if (std::is_same<Order, GrevLexOrder>::value) return terms.begin()->second;
    return std::prev(terms.end())->second;
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::eraseLeadingTerm() {
    assert(!terms.empty());
    if (std::is_same<Order, GrevLexOrder>::value) terms.erase(terms.begin());
    else terms.erase(std::prev(terms.end()));
}
template <class Coef, class Order>
Term<Coef> Polynomial<Coef, Order>::popLeadingTerm() {
    auto it = std::prev(terms.end());
    if (std::is_same<Order, GrevLexOrder>::value) it = terms.begin();
    Term<Coef> t{it->first, it->second};
    terms.erase(it);
    return t;
}
}  // namespace sci_calc
