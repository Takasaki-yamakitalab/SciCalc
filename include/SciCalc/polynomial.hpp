#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include "sci_calc_define.hpp"
#include <map>

namespace sci_calc {

class Monomial {
   private:
    std::vector<int> m_exp;
    int m_totalDegree;

   public:
    Monomial(const std::vector<int>& e);
    Monomial(const Monomial& other) = default;

    int degree() const;
    int update_degree();
    bool isOne() const;
    bool divides(const Monomial& rhs) const;
    Monomial operator*(const Monomial& rhs) const;
    Monomial operator/(const Monomial& rhs) const;
    Monomial& operator*=(const Monomial& rhs);
    Monomial& operator/=(const Monomial& rhs);
    bool operator==(const Monomial& rhs) const;
    bool operator!=(const Monomial& rhs) const;
    const int& operator[](size_t i) const;

    const size_t size() const { return m_exp.size(); }
    const int& totalDegree() const { return m_totalDegree; }
    const int& exp(size_t index) const {
        assert(index < m_exp.size());
        return m_exp[index];
    }
    void add_exp(size_t index, int e) {
        assert(index < m_exp.size());
        m_exp[index] += e;
        m_totalDegree += e;
    }
};

struct LexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};
struct GrLexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};
struct GrevLexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};

template <class Coef>
struct Term {
    Monomial mono;
    Coef coef;

    bool isZero() const;

    bool operator==(const Term<Coef>& t) const;
    bool operator!=(const Term<Coef>& t) const;
    Term<Coef> operator-() const;
};

template <class Coef, class Order = GrevLexOrder>
struct Polynomial {
    using Map = std::map<Monomial, Coef, Order>;
    Map terms;

    Polynomial() = default;
    Polynomial(const Term<Coef>& t) { addTerm(t); }

    bool isZero() const;
    bool isConstant() const;
    bool isMonomial() const;
    int degree() const;
    Term<Coef> leadingTerm() const;
    const Monomial& leadingMonomial() const;
    const Coef& leadingCoefficient() const;
    int leadingDegree() const;
    Coef coefficient(const Monomial& m) const;
    void eraseLeadingTerm();
    Term<Coef> popLeadingTerm();
    Polynomial<Coef, Order> leadingPolynomial() const;
    void normalize();
    void clear();
    void addTerm(const Monomial& m, const Coef& c);
    void addTerm(const Term<Coef>& t);
    bool reducibleBy(const Polynomial& g) const;
    Polynomial& reduceOnceBy(const Polynomial&);
    Polynomial& reduceBy(const Polynomial&);
    Polynomial& normalForm(const std::vector<Polynomial>& g);

    Polynomial& operator+=(const Polynomial& rhs);
    Polynomial& operator+=(const Term<Coef>& rhs);
    Polynomial& operator-=(const Polynomial& rhs);
    Polynomial& operator*=(const Coef& c);
    Polynomial& operator*=(const Term<Coef>& t);
    Polynomial operator-() const;
};
using PolyLex = Polynomial<Complex, LexOrder>;
using PolyGrLex = Polynomial<Complex, GrLexOrder>;
using PolyGrevLex = Polynomial<Complex, GrevLexOrder>;

template <class Coef, class Order = GrevLexOrder>
struct DivisionResult {
    std::vector<Polynomial<Coef, Order>> Q;
    Polynomial<Coef, Order> R;
};

Monomial gcd(const Monomial& a, const Monomial& b);
Monomial lcm(const Monomial& a, const Monomial& b);

template <class Coef>
Term<Coef> operator*(const Term<Coef>& a, const Term<Coef>& b);
template <class Coef>
Term<Coef> operator/(const Term<Coef>& a, const Term<Coef>& b);
template <class Coef>
Term<Coef> operator*(Term<Coef> t, const Coef& c);
template <class Coef>
Term<Coef> operator*(const Coef& c, Term<Coef> t);

template <class Coef, class Order>
Polynomial<Coef, Order> operator+(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b);
template <class Coef, class Order>
Polynomial<Coef, Order> operator-(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b);
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(const Polynomial<Coef, Order>& a, const Polynomial<Coef, Order>& b);
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(Polynomial<Coef, Order> p, const Term<Coef>& t);
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(Polynomial<Coef, Order> p, const Coef& c);
template <class Coef, class Order>
Polynomial<Coef, Order> operator*(const Coef& c, Polynomial<Coef, Order> p);
template <class Coef, class Order>
Polynomial<Coef, Order> differential(const Polynomial<Coef, Order>& p, int var);
template <class Coef, class Order>
DivisionResult<Coef, Order> divide(Polynomial<Coef, Order> F, const std::vector<Polynomial<Coef, Order>>& G);

template <class Coef, class Order>
Polynomial<Coef, Order> sPolynomial(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g);

template <class Coef, class Order>
std::vector<std::pair<Polynomial<Coef, Order>, int>> factorize(Polynomial<Coef, Order> P);

}  // namespace sci_calc

#endif
