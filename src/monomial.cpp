#include <SciCalc/sci_calc.hpp>
#include <complex>

namespace sci_calc {

template <class Coef>
Coef coefficientGCD(const Coef& a, const Coef& b) {
    while (b != Coef(b)) {
        Coef temp = b;
        b = fmod(a, b);
        a = temp;
    }
}
template <class T>
std::complex<T> coefficientGCD(const std::complex<T>& a, const std::complex<T>& b) {
    return {T(1), T(0)};
}

template <class Coef>
Coef content(const std::vector<Coef>& c) {
    assert(!c.empty());
    Coef g = abs(c.front());
    for (size_t i = 1; i < c.size(); i++) {
        g = coefficientGCD(g, abs(c[i]));
    }
    return g;
}

template <class T>
std::complex<T> content(const std::vector<std::complex<T>>& c) {
    return {T(1), T(0)};
}

int64_t permutation(const int64_t& n, const int64_t& r) {
    int64_t ans = 1;
    for (int64_t i = n; i > n - r; i++) {
        ans *= i;
    }
    return ans;
}
int64_t combination(const int64_t& n, const int64_t& r) {
    int64_t rr = 1;
    for (int64_t i = r; i >= 1; i++) {
        rr *= i;
    }
    return permutation(n, r) / rr;
}

template <class Coef>
Coef pow(Coef c, int64_t n) {
    if (n == 0) return Coef(0);
    if (n == 1) return c;
    for (size_t i = 0; i < n; i++) {
        c *= c;
    }
    return c;
}

// Monomial member functions
Monomial::Monomial(const size_t dim) : m_exp(dim, 0), m_totalDegree(0) {}
Monomial::Monomial(const size_t dim, const int64_t e) : m_exp(dim, e), m_totalDegree(dim * e) {}
Monomial::Monomial(const std::vector<int64_t>& e) : m_exp(e) {
    m_totalDegree = 0;
    for (size_t i = 0; i < e.size(); i++) {
        m_totalDegree += e[i];
    }
}
int64_t Monomial::degree() const { return m_totalDegree; }
int64_t Monomial::update_degree() {
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
    assert(m_exp.size() == rhs.m_exp.size());
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
const int64_t& Monomial::operator[](size_t i) const { return m_exp[i]; }

Monomial gcd(const Monomial& a, const Monomial& b) {
    assert(a.size() == b.size());
    std::vector<int64_t> ret(a.size(), 0);
    for (size_t i = 0; i < ret.size(); i++) {
        ret[i] = std::min(a.exp(i), b.exp(i));
    }
    return Monomial(ret);
}
Monomial lcm(const Monomial& a, const Monomial& b) {
    assert(a.size() == b.size());
    std::vector<int64_t> ret(a.size(), 0);
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
