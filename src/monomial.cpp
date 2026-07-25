#include <SciCalc/sci_calc.hpp>

namespace sci_calc {

// Monomial member functions
Monomial::Monomial(const std::vector<int>& e) : m_exp(e) {
    m_totalDegree = 0;
    for (size_t i = 0; i < e.size(); i++) {
        m_totalDegree += e[i];
    }
}
int Monomial::degree() const {
    return m_totalDegree;
}
int Monomial::update_degree() {
    m_totalDegree = 0;
    for (size_t i = 0; i < m_exp.size(); i++) {
        m_totalDegree += m_exp[i];
    }
    return m_totalDegree;
}
bool Monomial::isOne() const {
    for (auto e : m_exp) {
        if (e != 0) return false;
    }
    return true;
}
bool Monomial::divides(const Monomial& rhs) const {
    assert(m_exp.size() == rhs.m_exp.size());
    for (size_t i = 0; i < m_exp.size(); i++) {
        if (m_exp[i] > rhs.m_exp[i]) return false;
    }
    return true;
}
Monomial Monomial::operator*(const Monomial& rhs) const {
    Monomial r = *this;
    return r *= rhs;
}
Monomial Monomial::operator/(const Monomial& rhs) const {
    assert(rhs.divides(*this));
    Monomial r = *this;
    return r /= rhs;
}
Monomial& Monomial::operator*=(const Monomial& rhs) {
    for (size_t i = 0; i < m_exp.size(); i++) {
        m_exp[i] += rhs.m_exp[i];
        m_totalDegree += rhs.m_exp[i];
    }
    return *this;
}
Monomial& Monomial::operator/=(const Monomial& rhs) {
    assert(rhs.divides(*this));
    for (size_t i = 0; i < m_exp.size(); i++) {
        m_exp[i] -= rhs.m_exp[i];
        m_totalDegree -= rhs.m_exp[i];
    }
    return *this;
}
bool Monomial::operator==(const Monomial& rhs) const { return m_exp == rhs.m_exp; }
bool Monomial::operator!=(const Monomial& rhs) const { return m_exp != rhs.m_exp; }
const int& Monomial::operator[](size_t i) const {
    return m_exp[i];
}

Monomial gcd(const Monomial& a, const Monomial& b) {
    assert(a.size() == b.size());
    std::vector<int> ret(a.size(), 0);
    for (size_t i = 0; i < ret.size(); i++) {
        ret[i] = std::min(a.exp(i), b.exp(i));
    }
    return Monomial(ret);
}
Monomial lcm(const Monomial& a, const Monomial& b) {
    assert(a.size() == b.size());
    std::vector<int> ret(a.size(), 0);
    for (size_t i = 0; i < ret.size(); i++) {
        ret[i] = std::max(a.exp(i), b.exp(i));
    }
    return Monomial(ret);
}

// Term member functions
template <class Coef>
bool Term<Coef>::isZero() const {
    return coef == Coef(0);
}
template <class Coef>
bool Term<Coef>::operator==(const Term<Coef>& t) const {
    if (mono != t.mono) return false;
    return coef == t.coef;
}
template <class Coef>
bool Term<Coef>::operator!=(const Term<Coef>& t) const {
    return !(*this == t);
}
template <class Coef>
Term<Coef> Term<Coef>::operator-() const {
    Term<Coef> ret = *this;
    ret.coef = -ret.coef;
    return ret;
}

template <class Coef>
Term<Coef> operator*(const Term<Coef>& a, const Term<Coef>& b) {
    return {a.mono * b.mono, a.coef * b.coef};
}
template <class Coef>
Term<Coef> operator/(const Term<Coef>& a, const Term<Coef>& b) {
    assert(b.mono.divides(a.mono));
    return {a.mono / b.mono, a.coef / b.coef};
}
template <class Coef>
Term<Coef> operator*(Term<Coef> t, const Coef& c) {
    t.coef *= c;
    return t;
}
template <class Coef>
Term<Coef> operator*(const Coef& c, Term<Coef> t) {
    t.coef *= c;
    return t;
}

}  // namespace sci_calc
