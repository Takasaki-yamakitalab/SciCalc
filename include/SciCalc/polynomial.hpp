#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include "sci_calc_define.hpp"
#include <map>

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

template <class Coef, class Order = LexOrder>
struct Polynomial {
    using Map = std::map<Monomial, Coef, Order>;
    Map terms;

    Polynomial() = default;
    Polynomial(const Term<Coef>& t) { addTerm(t); }

    bool isZero() const;
    bool isConstant() const;
    bool isMonomial() const;
    int64_t degree() const;
    int64_t degree(const int& var) const;
    Term<Coef> leadingTerm() const;
    const Monomial& leadingMonomial() const;
    const Coef& leadingCoefficient() const;
    int64_t leadingDegree() const;
    Coef coefficient(const Monomial& m) const;
    void eraseLeadingTerm();
    Term<Coef> popLeadingTerm();
    Polynomial<Coef, Order> leadingPolynomial() const;
    void normalize();
    void clear();
    void addTerm(const Monomial& m, const Coef& c);
    void addTerm(const Term<Coef>& t);
    bool reducibleBy(const Polynomial& g) const;
    Polynomial& reduceOnceBy(const Polynomial& g);
    Polynomial& reduceBy(const Polynomial& g);
    Polynomial& normalForm(const std::vector<Polynomial>& G);

    bool operator==(const Polynomial& rhs) const;
    Polynomial& operator+=(const Polynomial& rhs);
    Polynomial& operator+=(const Term<Coef>& rhs);
    Polynomial& operator-=(const Polynomial& rhs);
    Polynomial& operator*=(const Coef& c);
    Polynomial& operator*=(const Term<Coef>& t);
    Polynomial& operator*=(const Polynomial& rhs);
    Polynomial& operator/=(const Coef& c);
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

template <class Coef, class Order>
struct SquareFreeFactor {
    Polynomial<Coef, Order> poly;
    uint64_t multiplicity;
};

template <class Coef, class Order>
struct HenselFactor {
    Polynomial<Coef, Order> factor;
    Polynomial<Coef, Order> cofactor;
};

template <class Coef, class Order>
struct ExtendEuclidResult {
    Polynomial<Coef, Order> gcd;
    Polynomial<Coef, Order> s;
    Polynomial<Coef, Order> t;
};

template <class Coef, class Order>
struct FactorNode {
    using Poly = Polynomial<Coef, Order>;

    Poly factor;
    std::unique_ptr<FactorNode> left;
    std::unique_ptr<FactorNode> right;

    bool isLeaf() const { return !left && !right; }
};

template <class Coef>
Coef content(const std::vector<Coef>& c);
template <class Coef>
std::complex<Coef> content(const std::vector<std::complex<Coef>>& c);

int64_t permutation(const int64_t& n, const int64_t& r);
int64_t combination(const int64_t& n, const int64_t& r);

template <class Coef>
Coef pow(Coef c, int64_t n);

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
Polynomial<Coef, Order> primitivePart(Polynomial<Coef, Order> p);
template <class Coef, class Order>
Polynomial<Coef, Order> pow(Polynomial<Coef, Order> p, int64_t e);
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(const Polynomial<Coef, Order>& f, int var, const Polynomial<Coef, Order>& g);
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(const Polynomial<Coef, Order>& f, int var, const Coef& value);
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(Polynomial<Coef, Order> f, const std::vector<Polynomial<Coef, Order>>& g);
template <class Coef, class Order>
DivisionResult<Coef, Order> pseudoDivide(Polynomial<Coef, Order> F, const std::vector<Polynomial<Coef, Order>>& G);
template <class Coef, class Order>
Polynomial<Coef, Order> pseudoRemainder(const Polynomial<Coef, Order>& F, const Polynomial<Coef, Order>& G);
template <class Coef, class Order>
Polynomial<Coef, Order> polynomialGCD(Polynomial<Coef, Order> F, Polynomial<Coef, Order> G);

template <class Coef, class Order>
Polynomial<Coef, Order> sPolynomial(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g);

template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> buchberger(std::vector<Polynomial<Coef, Order>> G);
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> GroebnerBasis(std::vector<Polynomial<Coef, Order>> G);
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> reduceGroebnerBasis(std::vector<Polynomial<Coef, Order>> G);

template <class Coef, class Order>
int findReducer(const std::vector<Polynomial<Coef, Order>>& G, const Monomial& m);

template <class Coef, class Order>
Polynomial<Coef, Order> multiDerivativeGCD(Polynomial<Coef, Order> f);

template <class Coef, class Order>
std::vector<SquareFreeFactor<Coef, Order>> squareFreeDecomposition(Polynomial<Coef, Order> f);
template <class RNG, class Coef, class Order>
Polynomial<Coef, Order> randomEvaluation(const Polynomial<Coef, Order>& f, std::vector<Coef>& values, RNG& rng,
                                         int bound = 10);
template <class Coef, class Order>
bool evaluationIsGood(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g);

template <class Coef, class Order>
DivisionResult<Coef, Order> divideByLinearFactor(const Polynomial<Coef, Order>& p, int var, Coef a);
template <class Coef, class Order>
HenselFactor<Coef, Order> henselLiftStep(const Polynomial<Coef, Order>& f, Polynomial<Coef, Order>& g,
                                         Polynomial<Coef, Order>& h, const Polynomial<Coef, Order>& s,
                                         const Polynomial<Coef, Order>& t, int var, Coef a, int64_t precision);

template <class Coef, class Order>
Polynomial<Coef, Order> product(const std::vector<Polynomial<Coef, Order>>& factors);
template <class Coef, class Order>
Polynomial<Coef, Order> product(const std::vector<Polynomial<Coef, Order>>& factors, size_t begin, size_t end);
template <class Coef, class Order>
void henselLiftTree(const Polynomial<Coef, Order>& f, std::vector<Polynomial<Coef, Order>>& factors, int var,
                    const Coef& a);
template <class Coef, class Order>
void henselLiftTree(const Polynomial<Coef, Order>& f, FactorNode<Coef, Order>& node, int var, const Coef& a);

template <class Coef, class Order>
std::unique_ptr<FactorNode<Coef, Order>> makeLeaf(const Polynomial<Coef, Order>& f);
template <class Coef, class Order>
std::unique_ptr<FactorNode<Coef, Order>> buildFactorTree(const std::vector<Polynomial<Coef, Order>>& factors,
                                                         size_t begin, size_t end);

template <class Coef, class Order>
int kroneckerBase(const Polynomial<Coef, Order>& f);
template <class Coef, class Order>
Polynomial<Coef, LexOrder> kroneckerSubstittution(const Polynomial<Coef, Order>& f, int64_t B);
template <class Coef>
Polynomial<Coef, LexOrder> inverseKronecker(const Polynomial<Coef, LexOrder>& f, int64_t B, size_t var_num);

template <class Coef, class Order>
bool isFactor(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g);

template <class Coef, class Order>
Coef content(const Polynomial<Coef, Order>& f);

template <class Coef, class Order>
void collectLeaves(FactorNode<Coef, Order>& node, std::vector<Polynomial<Coef, Order>>& ret);
template <class Coef, class Order>
Polynomial<int, Order> modPolynomial(const Polynomial<Coef, Order>& f, int p);
template <class Order>
Polynomial<int, Order> derivativeMod(const Polynomial<int, Order>& f, int p);
template <class Order>
Polynomial<int, Order> powModPolynomial(Polynomial<int, Order> a, int64_t n, const Polynomial<int, Order>& mod, int p);
template <class Order>
int findGoodPrime(const Polynomial<int, Order>& f);
std::vector<std::vector<int>> nullSpace(std::vector<std::vector<int>> A, int p);
template <class Order>
std::vector<std::vector<int>> berlekampMatrix(const Polynomial<int, Order>& f, int p);
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> factorSquareFree(Polynomial<Coef, Order> f);
template <class Coef, class Order>
std::vector<std::pair<Polynomial<Coef, Order>, int64_t>> factorize(Polynomial<Coef, Order> f);

}  // namespace sci_calc

#endif
