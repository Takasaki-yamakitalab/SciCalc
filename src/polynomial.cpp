#include <corecrt_wstring.h>
#include <SciCalc/sci_calc.hpp>

namespace sci_calc {

// Polynomial member functions
template <class Coef, class Order>
bool Polynomial<Coef, Order>::isZero() const {
    return terms.empty();
}
template <class Coef, class Order>
bool Polynomial<Coef, Order>::isConstant() const {
    if (terms.empty()) return true;
    if (terms.size() != 1) return false;
    return leadingMonomial().degree() == 0;
}
template <class Coef, class Order>
bool Polynomial<Coef, Order>::isMonomial() const {
    return terms.size() == 1;
}
template <class Coef, class Order>
int Polynomial<Coef, Order>::degree() const {
    if (isZero()) return -1;
    return leadingDegree();
}
template <class Coef, class Order>
Term<Coef> Polynomial<Coef, Order>::leadingTerm() const {
    assert(!terms.empty());
    auto it = std::prev(terms.end());
    return {it->first, it->second};
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::normalize() {
    for (auto it = terms.begin(); it != terms.end();) {
        if (it->second == Coef(0)) {
            it = terms.erase(it);
        } else {
            it++;
        }
    }
}
template <class Coef, class Order>
const Monomial& Polynomial<Coef, Order>::leadingMonomial() const {
    assert(!terms.empty());
    return std::prev(terms.end())->first;
}
template <class Coef, class Order>
const Coef& Polynomial<Coef, Order>::leadingCoefficient() const {
    assert(!terms.empty());
    return std::prev(terms.end())->second;
}
template <class Coef, class Order>
int Polynomial<Coef, Order>::leadingDegree() const {
    return leadingMonomial().degree();
}
template <class Coef, class Order>
Coef Polynomial<Coef, Order>::coefficient(const Monomial& m) const {
    auto it = terms.find(m);
    if (it == terms.end()) return Coef(0);
    return it->second;
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::eraseLeadingTerm() {
    assert(!terms.empty());
    terms.erase(std::prev(terms.end()));
}
template <class Coef, class Order>
Term<Coef> Polynomial<Coef, Order>::popLeadingTerm() {
    auto it = std::prev(terms.end());
    Term<Coef> t{it->first, it->second};
    terms.erase(it);
    return t;
}
template <class Coef, class Order>
Polynomial<Coef, Order> Polynomial<Coef, Order>::leadingPolynomial() const {
    return Polynomial(leadingTerm());
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::clear() {
    terms.clear();
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::addTerm(const Monomial& m, const Coef& c) {
    if (c == Coef(0)) return;
    auto it = terms.find(m);
    if (it == terms.end()) {
        terms.emplace(m, c);
        return;
    }
    it->second += c;
    if (it->second == Coef(0)) terms.erase(it);
}
template <class Coef, class Order>
void Polynomial<Coef, Order>::addTerm(const Term<Coef>& t) {
    addTerm(t.mono, t.coef);
}
template <class Coef, class Order>
bool Polynomial<Coef, Order>::reducibleBy(const Polynomial& g) const {
    if (isZero()) return false;
    if (g.isZero()) return false;
    return g.leadingMonomial().divides(leadingMonomial());
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::reduceOnceBy(const Polynomial<Coef, Order>& g) {
    assert(!g.isZero());
    auto t = leadingTerm() / g.leadingTerm();
    *this -= g * t;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::reduceBy(const Polynomial<Coef, Order>& g) {
    assert(!g.isZero());
    while (reducibleBy(g)) {
        reduceOnceBy(g);
    }
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::normalForm(const std::vector<Polynomial<Coef, Order>>& G) {
    bool changed;
    do {
        changed = false;
        for (const auto& g : G) {
            if (reducibleBy(g)) {
                reduceOnceBy(g);
                changed = true;
                break;
            }
        }
    } while (changed);
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator+=(const Polynomial<Coef, Order>& rhs) {
    for (auto& t : rhs.terms) {
        addTerm(t.first, t.second);
    }
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator+=(const Term<Coef>& rhs) {
    addTerm(rhs);
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator-=(const Polynomial<Coef, Order>& rhs) {
    for (auto& t : rhs.terms) {
        addTerm(t.first, -t.second);
    }
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator*=(const Coef& c) {
    if (c == Coef(0)) {
        clear();
        return *this;
    }
    for (auto& term : terms) {
        term.second *= c;
    }
    normalize();
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator*=(const Term<Coef>& t) {
    Polynomial<Coef, Order> tmp;
    for (auto& term : terms) {
        tmp.addTerm(term.first * t.mono, term.second * t.coef);
    }
    terms.swap(tmp.terms);
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order> Polynomial<Coef, Order>::operator-() const {
    Polynomial<Coef, Order> ret = *this;
    for (auto& t : ret.terms) {
        t.second = -t.second;
    }
    return ret;
}

template <class Coef, class Order>
Polynomial<Coef, Order> operator+(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b) {
    Polynomial<Coef, Order> ret = a;
    for (auto itr = b.terms.begin(); itr != b.terms.end(); itr++) {
        ret.addTerm(itr->first, itr->second);
    }
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> operator-(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b) {
    Polynomial<Coef, Order> ret = a;
    for (auto itr = b.terms.begin(); itr != b.terms.end(); itr++) {
        ret.addTerm(itr->first, -itr->second);
    }
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b) {
    Polynomial<Coef, Order> ret;
    for (auto itr1 = a.terms.begin(); itr1 != a.terms.end(); itr1++) {
        for (auto itr2 = b.terms.begin(); itr2 != b.terms.end(); itr2++) {
            ret.addTerm(itr1->first * itr2->first, itr1->second * itr2->second);
        }
    }
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(Polynomial<Coef, Order> p, const Term<Coef>& t) {
    p *= t;
    return p;
}
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(Polynomial<Coef, Order> p, const Coef& c) {
    Polynomial<Coef, Order> ret = p;
    ret *= c;
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(const Coef& c, Polynomial<Coef, Order> p) {
    Polynomial<Coef, Order> ret = p;
    ret *= c;
    return ret;
}

template <class Coef, class Order>
Polynomial<Coef, Order> differential(const Polynomial<Coef, Order>& p, int var) {
    Polynomial<Coef, Order> ret;
    for (auto& term : p.terms) {
        if (term.first.exp[var] == 0) continue;
        Monomial mono = term.first;
        Coef coeff = term.second * Coef(mono.exp(var));
        mono.add_exp(var, -1);
        ret.addTerm(mono, coeff);
    }
    return ret;
}

template <class Coef, class Order>
DivisionResult<Coef, Order> divide(Polynomial<Coef, Order> F, const std::vector<Polynomial<Coef, Order>>& G) {
    std::vector<Polynomial<Coef, Order>> Q(G.size());
    Polynomial<Coef, Order> R;

    while (!F.isZero()) {
        bool reduced = false;
        for (size_t i = 0; i < G.size(); i++) {
            if (G[i].isZero()) continue;
            if (!F.reducibleBy(G[i])) continue;

            while (F.reducibleBy(G[i])) {
                Q[i] += F.leadingTerm() / G[i].leadingTerm();
                F.reduceOnceBy(G[i]);
            }
            reduced = true;
            break;
        }

        if (!reduced) {
            R += F.leadingTerm();
            F.eraseLeadingTerm();
        }
    }

    return {Q, R};
}

template <class Coef, class Order>
Polynomial<Coef, Order> sPolynomial(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g) {
    assert(!f.isZero() && !g.isZero());
    Monomial l = lcm(f.leadingMonomial(), g.leadingMonomial());
    Term<Coef> common{l, Coef(1)};
    return f * (common / f.leadingTerm()) - g * (common / g.leadingTerm());
}
}  // namespace sci_calc
