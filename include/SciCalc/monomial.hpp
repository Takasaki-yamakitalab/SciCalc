#ifndef MONOMIAL_HPP
#define MONOMIAL_HPP

#include <vector>
#include <cassert>

namespace sci_calc {

class Monomial {
   private:
    std::vector<int64_t> m_exp;
    int64_t m_totalDegree;

   public:
    Monomial(const size_t dim);
    Monomial(const size_t dim, const int64_t e);
    Monomial(const std::vector<int64_t>& e);
    Monomial(const Monomial& other) = default;

    int64_t degree() const;
    int64_t update_degree();
    bool isOne() const;
    bool divides(const Monomial& rhs) const;
    Monomial operator*(const Monomial& rhs) const;
    Monomial operator/(const Monomial& rhs) const;
    Monomial& operator*=(const Monomial& rhs);
    Monomial& operator/=(const Monomial& rhs);
    bool operator==(const Monomial& rhs) const;
    bool operator!=(const Monomial& rhs) const;
    const int64_t& operator[](size_t i) const;

    const size_t size() const { return m_exp.size(); }
    const int64_t& totalDegree() const { return m_totalDegree; }
    const int64_t& exp(size_t index) const {
        assert(index < m_exp.size());
        return m_exp[index];
    }
    void add_exp(size_t index, int64_t e) {
        assert(index < m_exp.size());
        m_exp[index] += e;
        m_totalDegree += e;
    }
    void set_exp(size_t index, int64_t e) {
        assert(index < m_exp.size());
        m_totalDegree += e - m_exp[index];
        m_exp[index] = e;
    }
};

Monomial gcd(const Monomial& a, const Monomial& b);
Monomial lcm(const Monomial& a, const Monomial& b);

}

#endif
