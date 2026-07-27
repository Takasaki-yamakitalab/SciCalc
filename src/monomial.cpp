#include "SciCalc/monomial.hpp"

namespace sci_calc {

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

}  // namespace sci_calc
