#include <SciCalc/sci_calc.hpp>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <memory>
#include <random>

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
int64_t Polynomial<Coef, Order>::degree() const {
    if (isZero()) return -1;
    int64_t deg = 0;
    for (auto it = terms.begin(); it != terms.end(); it++) {
        int64_t n = terms.mono.degree();
        if (n > deg) deg = n;
    }
    return deg;
}
template <class Coef, class Order>
int64_t Polynomial<Coef, Order>::degree(const int& var) const {
    if (isZero()) return -1;
    int64_t deg = 0;
    for (auto it = terms.begin(); it != terms.end(); it++) {
        int64_t n = terms.mono.exp(var);
        if (n > deg) deg = n;
    }
    return deg;
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
int64_t Polynomial<Coef, Order>::leadingDegree() const {
    return leadingMonomial().degree();
}
template <class Coef, class Order>
Polynomial<Coef, Order> Polynomial<Coef, Order>::leadingPolynomial() const {
    return Polynomial(leadingTerm());
}
template <class Coef, class Order>
Coef Polynomial<Coef, Order>::coefficient(const Monomial& m) const {
    auto it = terms.find(m);
    if (it == terms.end()) return Coef(0);
    return it->second;
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
    assert(reducibleBy(g));
    auto t = leadingTerm() / g.leadingTerm();
    *this -= g * t;
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::reduceBy(const Polynomial<Coef, Order>& g) {
    while (reducibleBy(g)) {
        reduceOnceBy(g);
    }
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::normalForm(const std::vector<Polynomial<Coef, Order>>& G) {
    auto tmp = divide(*this, G).R;
    terms.swap(tmp.terms);
    return *this;
}
template <class Coef, class Order>
bool Polynomial<Coef, Order>::operator==(const Polynomial<Coef, Order>& rhs) const {
    return terms == rhs.terms;
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
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator*=(const Polynomial<Coef, Order>& p) {
    auto temp = *this * p;
    terms.swap(temp.terms);
    return *this;
}
template <class Coef, class Order>
Polynomial<Coef, Order>& Polynomial<Coef, Order>::operator/=(const Coef& c) {
    assert(c != Coef(0));
    for (auto& term : terms) {
        term.second /= c;
    }
    normalize();
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

            Q[i] += F.leadingTerm() / G[i].leadingTerm();
            F.reduceOnceBy(G[i]);
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
Polynomial<Coef, Order> primitivePart(Polynomial<Coef, Order> p) {
    if (p.isZero()) return p;

    Coef c = content(p);

    if (c != Coef(1)) p /= c;

    return p;
}
template <class Coef, class Order>
DivisionResult<Coef, Order> pseudoDivide(Polynomial<Coef, Order> F, const std::vector<Polynomial<Coef, Order>>& G) {
    using Poly = Polynomial<Coef, Order>;
    DivisionResult<Coef, Order> ret;
    assert(!G.isZero());

    Poly R = F;
    Poly Q;

    int m = F.degree();
    int n = G.degree();

    if (m < n) {
        ret.R = F;
        ret.Q.push_back(Q);
        return ret;
    }

    Coef c = G.leadingCoefficient();
    while (!R.isZero() && R.degree() >= G.degree()) {
        Monomial u = R.leadingMonomial() / G.leadingMonomial();
        Coef a = R.leadingCoefficient();
        Term<Coef> t{u, a};
        Q *= c;
        Q += t;
        R *= c;
        R -= G * t;
    }
    ret.Q.push_back(Q);
    ret.R = R;
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> pseudoRemainder(const Polynomial<Coef, Order>& F, const Polynomial<Coef, Order>& G) {
    return pseudoDivide(F, G).R;
}
template <class Coef, class Order>
Polynomial<Coef, Order> polynomialGCD(Polynomial<Coef, Order> F, Polynomial<Coef, Order> G) {
    using Poly = Polynomial<Coef, Order>;

    if (F.isZero()) return G;
    if (G.isZero()) return F;

    while (!G.isZero()) {
        Poly R = pseudoRemainder(F, G);
        R = primitivePart(R);
        F = G;
        G = R;
    }
    if (!F.isZero()) F *= Coef(1) / F.leadingCoefficient();

    return primitivePart(F);
}
template <class Coef, class Order>
Polynomial<Coef, Order> pow(Polynomial<Coef, Order> p, int64_t e) {
    using Poly = Polynomial<Coef, Order>;
    assert(e >= 0);

    Poly r;
    r.addTerm(Monomial(std::vector<int64_t>(p.leadingMonomial().size(), 0)), Coef(1));
    while (e) {
        if (e & 1) r = r * p;
        p = p * p;
        e >>= 1;
    }
    return r;
}
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(const Polynomial<Coef, Order>& f, int var, const Polynomial<Coef, Order>& g) {
    using Poly = Polynomial<Coef, Order>;

    Poly ans;

    for (auto it = f.terms.begin(); it != f.terms.end(); ++it) {
        const Monomial& m = it->first;
        Coef c = it->second;

        Poly term;
        term.addTerm(Monomial(std::vector<int>(m.size(), 0)), c);

        for (size_t i = 0; i < m.size(); i++) {
            if (m.exp(i) == 0) continue;

            if ((int)i == var) {
                term = term * pow(g, m.exp(i));
            } else {
                std::vector<int64_t> e(m.size(), 0);
                e[i] = m.exp(i);

                Poly t;
                t.addTerm(Monomial(e), Coef(1));

                term = term * t;
            }
        }
        ans += term;
    }
    return ans;
}
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(const Polynomial<Coef, Order>& f, int var, const Coef& value) {
    using Poly = Polynomial<Coef, Order>;

    Poly ans;

    for (auto it = f.terms.begin(); it != f.terms.end(); ++it) {
        Monomial mono = it->first;
        Coef coef = it->second;

        int64_t e = mono.exp(var);

        Coef p = Coef(1);
        for (int i = 0; i < e; i++) p *= value;

        coef *= p;

        mono.add_exp(var, -e);
        ans.addTerm(mono, coef);
    }
    return ans;
}
template <class Coef, class Order>
Polynomial<Coef, Order> substitute(Polynomial<Coef, Order> f, const std::vector<Polynomial<Coef, Order>>& g) {
    assert(g.size() == f.leadingMonomial().size());

    for (int i = (int)g.size() - 1; i >= 0; i--) f = substitute(f, i, g[i]);

    return f;
}

template <class Coef, class Order>
Polynomial<Coef, Order> sPolynomial(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g) {
    assert(!f.isZero() && !g.isZero());
    Monomial l = lcm(f.leadingMonomial(), g.leadingMonomial());
    Term<Coef> common{l, Coef(1)};
    return f * (common / f.leadingTerm()) - g * (common / g.leadingTerm());
}

template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> buchberger(std::vector<Polynomial<Coef, Order>> G) {
    using Poly = Polynomial<Coef, Order>;
    std::deque<std::pair<int, int>> pairs;
    for (int i = 0; i < (int)G.size(); i++) {
        for (int j = i + 1; j < (int)G.size(); j++) {
            pairs.emplace_back(i, j);
        }
    }
    while (!pairs.empty()) {
        int i = pairs.front().first;
        int j = pairs.front().second;
        pairs.pop_front();

        if (gcd(G[i].leadingMonomial(), G[j].leadingMonomial()).isOne()) continue;

        Poly S = sPolynomial(G[i], G[j]);
        S.normalForm(G);
        if (S.isZero()) continue;

        bool exist = false;
        for (auto& g : G) {
            if (g == S) exist = true;
        }
        if (exist) continue;

        int k = (int)G.size();
        G.push_back(S);
        for (int t = 0; t < k; t++) {
            pairs.emplace_back(t, k);
        }
    }
    return G;
}

template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> reduceGroebnerBasis(std::vector<Polynomial<Coef, Order>> G) {
    using Poly = Polynomial<Coef, Order>;

    for (size_t i = 0; i < G.size();) {
        std::vector<Poly> others;
        for (size_t j = 0; j < G.size(); j++) {
            if (i != j) others.push_back(G[j]);
        }
        G[i].normalForm(others);
        if (G[i].isZero()) {
            G.erase(G.begin() + i);
            continue;
        }
        i++;
    }

    // make monic
    for (auto& g : G) {
        g *= Coef(1) / g.leadingCoefficient();
    }
    return G;
}

template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> groebnerBasis(std::vector<Polynomial<Coef, Order>> G) {
    G = buchberger(G);
    G = reduceGroebnerBasis(G);
    return G;
}

template <class Coef, class Order>
int findReducer(const std::vector<Polynomial<Coef, Order>>& G, const Monomial& m) {
    for (int i = 0; i < (int)G.size(); i++) {
        if (G[i].leadingMonomial().divides(m)) return i;
    }
    return -1;
}

template <class Coef, class Order>
Polynomial<Coef, Order> multiDerivativeGCD(Polynomial<Coef, Order> f) {
    using Poly = Polynomial<Coef, Order>;
    Poly g = f;
    int n = f.leadingMonomial().size();
    for (int i = 0; i < n; i++) {
        Poly df = differential(f, i);

        if (df.isZero()) continue;

        g = polynomialGCD(g, df);

        if (g.isConstant()) break;
    }
    return g;
}

template <class Coef, class Order>
std::vector<SquareFreeFactor<Coef, Order>> squareFreeDecomposition(Polynomial<Coef, Order> f) {
    using Poly = Polynomial<Coef, Order>;
    std::vector<SquareFreeFactor<Coef, Order>> ans;

    Poly g = multiDerivativeGCD(f);
    Poly w = divide(f, {g}).Q[0];

    uint64_t i = 1;
    while (!w.isConstant()) {
        Poly y = polynomialGCD(w, g);
        Poly z = divide(w, {y}).Q[0];
        if (!z.isConstant()) ans.push_back({z, i});
        w = y;
        g = divide(g, {y}).Q[0];
        i++;
    }

    if (!g.isConstant()) {
        ans.push_back({g, i});
    }

    return ans;
}

template <class RNG, class Coef, class Order>
Polynomial<Coef, Order> randomEvaluation(const Polynomial<Coef, Order>& f, std::vector<Coef>& values, RNG& rng,
                                         int bound) {
    using Poly = Polynomial<Coef, Order>;

    const int vars = f.leadingMonomial().size();

    values.resize(vars);

    values[0] = Coef(0);

    std::uniform_int_distribution<int> dist(-bound, bound);

    Poly g = f;

    for (int i = 1; i < vars; i++) {
        Coef a;

        do {
            a = Coef(dist(rng));
        } while (a == Coef(0));

        values[i] = a;

        g = substitute(g, i, a);
    }

    return g;
}
template <class Coef, class Order>
bool evaluationIsGood(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g) {
    if (g.degree() != f.degree()) return false;

    auto d = differential(g, 0);

    if (!polynomialGCD(g, d).isConstant()) return false;

    return true;
}

template <class Coef, class Order>
DivisionResult<Coef, Order> divideByLinearFactor(const Polynomial<Coef, Order>& p, int var, Coef a) {
    using T = Term<Coef>;
    std::vector<T> termsVec;
    int64_t max_n = 0;
    for (auto it = p.terms.begin(); it != p.terms.end(); it++) {
        termsVec.push_back({it->first, it->second});
        if (it->first.exp(var) > max_n) max_n = it->first.exp(var);
    }

    std::vector<std::vector<T>> termsMat(max_n + 1);
    for (auto& t : termsVec) {
        int64_t n = t.mono.exp(var);
        termsMat[max_n - n].push_back(t);
    }

    for (int i = 0; i < (int)termsMat.size(); i++) {
        if (i != (int)termsMat.size() - 1) {
            for (int j = 0; j < (int)termsMat[i].size(); j++) {
                termsMat[i][j].mono.add_exp(var, -1);
            }
        }
        if (i == 0) continue;
        for (int j = 0; j < (int)termsMat[i - 1].size(); j++) {
            T t = termsMat[i - 1][j];
            if (i != (int)termsMat.size() - 1) t.mono.add_exp(var, -1);
            termsMat[i].push_back(a * t);
        }
    }

    Polynomial<Coef, Order> Q;
    Polynomial<Coef, Order> R;
    for (int i = 0; i < (int)termsMat.size() - 1; i++) {
        for (int j = 0; j < (int)termsMat[i].size(); j++) {
            Q.addTerm(termsMat[i][j]);
        }
    }
    for (int j = 0; j < (int)termsMat[termsMat.size() - 1].size(); j++) {
        R.addTerm(termsMat[termsMat.size() - 1][j]);
    }
    return DivisionResult<Coef, Order>{{Q}, R};
}
template <class Coef, class Order>
ExtendEuclidResult<Coef, Order> extendedEuclid(Polynomial<Coef, Order> f, Polynomial<Coef, Order> g) {
    using Poly = Polynomial<Coef, Order>;

    Poly s0, s1;
    Poly t0, t1;
    // s0 = 1
    s0.addTerm(Monomial(std::vector<int>(f.leadingMonomial().size(), 0)), Coef(1));
    // s1 = 0
    // t0 = 0
    // t1 = 1
    t1.addTerm(Monomial(std::vector<int>(f.leadingMonomial().size(), 0)), Coef(1));

    while (!g.isZero()) {
        DivisionResult<Coef, Order> div = divide(f, {g});
        Poly q = div.Q[0];
        Poly r = div.R;
        Poly s = s0 - q * s1;
        Poly t = t0 - q * t1;
        f = g;
        g = r;
        s0 = s1;
        s1 = s;
        t0 = t1;
        t1 = t;
    }

    Coef c = f.leadingCoefficient();
    if (c != Coef(1)) {
        f *= Coef(1) / c;
        s0 *= Coef(1) / c;
        t0 *= Coef(1) / c;
    }

    return {f, s0, t0};
}
template <class Coef, class Order>
HenselFactor<Coef, Order> henselLiftStep(const Polynomial<Coef, Order>& f, Polynomial<Coef, Order>& g,
                                         Polynomial<Coef, Order>& h, const Polynomial<Coef, Order>& s,
                                         const Polynomial<Coef, Order>& t, int var, Coef a, int64_t precision) {
    using Poly = Polynomial<Coef, Order>;

    size_t dim = f.leadingMonomial().size();
    Monomial mono(dim);
    Poly factor;
    for (int64_t n = precision; n >= 0; n--) {
        Monomial m = mono;
        m.add_exp(var, n);
        Coef c = combination(precision, n) * pow(-a, precision - n);
        factor.addTerm(m, c);
    }

    Poly E = f - g * h;
    for (int64_t i = 0; i < precision; i++) {
        E = divideByLinearFactor(E, var, a).Q[0];
    }
    g += (t * E) * factor;
    h += (s * E) * factor;
    return {g, h};
}
template <class Coef, class Order>
Polynomial<Coef, Order> shiftVariable(const Polynomial<Coef, Order>& f, int var, const Coef& a) {
    using Poly = Polynomial<Coef, Order>;
    Poly ret;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        Monomial mono = it->first;
        Coef c = it->second;
        int64_t e = mono.exp(var);
        mono.add_exp(var, -e);
        Poly base;
        base.addTerm(mono, c);
        Poly expand;
        for (int k = 0; k <= e; k++) {
            Monomial m = mono;
            m.add_exp(var, k);
            Coef coeff = c * Coef(combination(e, k)) * pow(a, e - k);
            expand.addTerm(m, coeff);
        }
        ret += expand;
    }
    return ret;
}
template <class Coef, class Order>
Polynomial<Coef, Order> truncateByPower(const Polynomial<Coef, Order>& f, int var, int k) {
    Polynomial<Coef, Order> r;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        if (it->first.exp(var) >= k) continue;
        r.addTerm(it->first, it->second);
    }
    return r;
}

template <class Coef, class Order>
void henselLift(const Polynomial<Coef, Order>& f, Polynomial<Coef, Order>& g, Polynomial<Coef, Order>& h, int var,
                const Coef& a) {
    auto ee = extendedEuclid(substitute(g, var, a), substitute(h, var, a));

    int precision = 1;
    while (precision <= f.degree(var)) {
        henselLiftStep(f, g, h, ee.s, ee.t, var, a, precision);
        precision *= 2;
    }
}

template <class Coef, class Order>
Polynomial<Coef, Order> product(const std::vector<Polynomial<Coef, Order>>& factors) {
    using Poly = Polynomial<Coef, Order>;
    if (factors.empty()) return Poly();
    Poly result = factors[0];
    for (size_t i = 1; i < factors.size(); i++) {
        result *= factors;
    }
    return result;
}
template <class Coef, class Order>
Polynomial<Coef, Order> product(const std::vector<Polynomial<Coef, Order>>& factors, size_t begin, size_t end) {
    using Poly = Polynomial<Coef, Order>;
    if (factors.empty()) return Poly();
    Poly result = factors[begin];
    for (size_t i = begin + 1; i < end; i++) {
        result *= factors;
    }
    return result;
}

template <class Coef, class Order>
void henselLiftTree(const Polynomial<Coef, Order>& f, std::vector<Polynomial<Coef, Order>>& factors, int var,
                    const Coef& a) {
    using Poly = Polynomial<Coef, Order>;
    if (factors.size() <= 1) return;

    size_t mid = factors.size() / 2;
    std::vector<Poly> leftFactors(factors.begin(), factors.begin() + mid);
    std::vector<Poly> rightFactors(factors.begin() + mid, factors.end());

    Poly g = product(leftFactors);
    Poly h = product(rightFactors);
    henselLift(f, g, h, var, a);

    henselLiftTree(g, leftFactors, var, a);
    henselLiftTree(h, rightFactors, var, a);

    factors.clear();

    factors.insert(factors.end(), leftFactors.begin(), leftFactors.end());
    factors.insert(factors.end(), rightFactors.begin(), rightFactors.end());
}

template <class Coef, class Order>
void henselLiftTree(const Polynomial<Coef, Order>& f, FactorNode<Coef, Order>& node, int var, const Coef& a) {
    using FNode = FactorNode<Coef, Order>;
    using Poly = Polynomial<Coef, Order>;

    if (node.isLeaf()) return;

    Poly g = node.left->factor;
    Poly h = node.right->factor;

    henselLift(f, g, h, var, a);

    node->left->factor = g;
    node->left->factor = h;

    henselLiftTree(g, *node->left, var, a);
    henselLiftTree(h, *node->right, var, a);

    // 念のため再計算
    node.factor = node.left->factor * node.right->factor;
}

template <class Coef, class Order>
std::unique_ptr<FactorNode<Coef, Order>> makeLeaf(const Polynomial<Coef, Order>& f) {
    auto node = std::make_unique<FactorNode<Coef, Order>>();
    node->factor = f;
    return node;
}

template <class Coef, class Order>
std::unique_ptr<FactorNode<Coef, Order>> buildFactorTree(const std::vector<Polynomial<Coef, Order>>& factors,
                                                         size_t begin, size_t end) {
    using FNode = FactorNode<Coef, Order>;
    using Poly = Polynomial<Coef, Order>;
    auto node = std::make_unique<FNode>();
    if (end - begin == 1) {
        node->factor = factors[begin];
        return node;
    }
    size_t mid = (begin + end) / 2;
    node->left = buildFactorTree(factors, begin, mid);
    node->right = buildFactorTree(factors, mid, end);
    node->factor = node->left->factor * node->right->factor;
    return node;
}

template <class Coef, class Order>
int kroneckerBase(const Polynomial<Coef, Order>& f) {
    int B = 1;
    size_t n = f.leadingMonomial().size();
    for (size_t i = 0; i < n; i++) {
        B = max(B, f.degree(i) + 1);
    }
    return B;
}
template <class Coef, class Order>
Polynomial<Coef, LexOrder> kroneckerSubstittution(const Polynomial<Coef, Order>& f, int64_t B) {
    using UniPoly = Polynomial<Coef, LexOrder>;
    UniPoly ret;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        int64_t e = 0;
        int64_t base = 1;
        for (size_t i = 0; i < it->first.size(); i++) {
            e += it->first.exp(i) * base;
            base *= B;
        }
        Monomial m(std::vector<int64_t>{e});

        ret.addTerm(m, it->second);
    }
    return ret;
}
template <class Coef>
Polynomial<Coef, LexOrder> inverseKronecker(const Polynomial<Coef, LexOrder>& f, int64_t B, size_t var_num) {
    using Poly = Polynomial<Coef, LexOrder>;
    Poly ret;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        int64_t e = it->first.exp(0);
        std::vector<int64_t> exps(var_num, 0);
        for (int i = 0; i < var_num; i++) {
            exps[i] = e % B;
            e /= B;
        }
        Monomial m(exps);
        ret.addTerm(m, it->second);
    }
    return ret;
}
template <class Coef, class Order>
bool isFactor(const Polynomial<Coef, Order>& f, const Polynomial<Coef, Order>& g) {
    auto div = divide(f, {g});
    if (!div.R.isZero()) return false;
    return div.Q[0] * g == f;
}

template <class Coef, class Order>
Coef content(const Polynomial<Coef, Order>& f) {
    std::vector<Coef> c;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        c.push_back(it->second);
    }
    return content(c);
}
template <class Coef, class Order>
void collectLeaves(FactorNode<Coef, Order>& node, std::vector<Polynomial<Coef, Order>>& ret) {
    if (node.isLeaf()) {
        ret.push_back(node.factor);
        return;
    }
    collectLeaves(*node.left, ret);
    collectLeaves(*node.right, ret);
}
template <class Coef, class Order>
Polynomial<int, Order> modPolynomial(const Polynomial<Coef, Order>& f, int p) {
    Polynomial<int, Order> ret;
    for (auto it = f.terms.begin(); it != f.terms.end(); it++) {
        int v = (int)(it->second + 0.5) % p;
        if (v < 0) v += p;
        if (v != 0) ret.addTerm(it->first, v);
    }
    return ret;
}
template <class Order>
Polynomial<int, Order> derivativeMod(const Polynomial<int, Order>& f, int p) {
    auto df = differential(f, 0);
    return modPolynomial(df, p);
}
template <class Order>
int findGoodPrime(const Polynomial<int, Order>& f) {
    static const int primes[] = {2,  3,  5,  7,  11, 13, 17, 19, 23, 29, 31, 37, 41,
                                 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};

    for (int p : primes) {
        auto fp = modPolynomial(f, p);

        // 次数が落ちる素数は除外
        if (fp.degree(0) != f.degree(0)) continue;

        auto df = derivativeMod(f, p);

        auto g = polynomialGCD(fp, df);

        // square-free
        if (g.isConstant()) return p;
    }

    throw std::runtime_error("no suitable prime");
}
template <class Order>
Polynomial<int, Order> powModPolynomial(Polynomial<int, Order> a, int64_t n, const Polynomial<int, Order>& mod, int p) {
    Polynomial<int, Order> result;
    result.addTerm(Monomial(1), 1);

    while (n > 0) {
        if (n & 1) {
            result = divide(result * a, {mod}).R;
        }
        a = divide(a * a, {mod}).R;
        n >>= 1;
    }
    return modPolynomial(result, p);
}
template <class Order>
std::vector<std::vector<int>> berlekampMatrix(const Polynomial<int, Order>& f, int p) {
    int n = f.degree(0);

    std::vector<std::vector<int>> Q(n, std::vector<int>(n));

    Polynomial<int, Order> x;
    x.addTerm(Monomial(std::vector<int64_t>{1}), 1);

    for (int j = 0; j < n; j++) {
        auto r = powModPolynomial(x, (int64_t)std::pow(p, j), f, p);

        for (auto it = r.terms.begin(); it != r.terms.end(); it++) {
            int i = it->first.exp(0);
            Q[i][j] = it->second;
        }
    }

    // Q-I
    for (int i = 0; i < n; i++) {
        Q[i][i] -= 1;
        Q[i][i] %= p;
        if (Q[i][i] < 0) Q[i][i] += p;
    }

    return Q;
}
std::vector<std::vector<int>> nullSpace(std::vector<std::vector<int>> A, int p) {
    // mod p Gauss elimination
    int m = A.size();
    int n = A[0].size();

    std::vector<int> pivot(n, -1);

    int row = 0;

    for (int col = 0; col < n && row < m; col++) {
        int sel = row;

        while (sel < m && A[sel][col] == 0) sel++;

        if (sel == m) continue;

        std::swap(A[sel], A[row]);

        int inv = 1;

        // pが素数なのでpowで逆元
        for (int i = 1; i < p; i++) {
            if ((A[row][col] * i) % p == 1) {
                inv = i;
                break;
            }
        }

        for (int j = 0; j < n; j++) A[row][j] = A[row][j] * inv % p;

        for (int i = 0; i < m; i++) {
            if (i == row) continue;

            int c = A[i][col];

            for (int j = 0; j < n; j++) {
                A[i][j] = (A[i][j] - c * A[row][j]) % p;

                if (A[i][j] < 0) A[i][j] += p;
            }
        }

        pivot[col] = row;

        row++;
    }

    std::vector<std::vector<int>> basis;

    for (int freeCol = 0; freeCol < n; freeCol++) {
        if (pivot[freeCol] != -1) continue;

        std::vector<int> v(n);

        v[freeCol] = 1;

        for (int col = 0; col < n; col++) {
            if (pivot[col] != -1) {
                v[col] = -A[pivot[col]][freeCol];

                v[col] %= p;

                if (v[col] < 0) v[col] += p;
            }
        }

        basis.push_back(v);
    }

    return basis;
}
template <class Order>
std::vector<Polynomial<int, Order>> berlekampFactor(Polynomial<int, Order> f, int p) {
    std::vector<Polynomial<int, Order>> ret;

    f = modPolynomial(f, p);

    auto basis = nullSpace(berlekampMatrix(f, p), p);

    if (basis.size() == 1) {
        ret.push_back(f);
        return ret;
    }

    // 今は最初の基底だけ利用
    for (auto& v : basis) {
        Polynomial<int, Order> b;

        for (int i = 0; i < v.size(); i++) {
            if (v[i] != 0) {
                b.addTerm(Monomial(std::vector<int64_t>{i}), v[i]);
            }
        }

        for (int a = 0; a < p; a++) {
            Polynomial<int, Order> tmp = b;
            tmp.addTerm(Monomial(b.leadingMonomial().size()), -a);
            auto g = polynomialGCD(f, tmp);

            if (!g.isConstant() && g != f) {
                auto left = berlekampFactor(g, p);
                auto right = berlekampFactor(divide(f, {g}).Q[0], p);
                ret.insert(ret.end(), left.begin(), left.end());
                ret.insert(ret.end(), right.begin(), right.end());
                return ret;
            }
        }
    }
    ret.push_back(f);
    return ret;
}
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> factorSquareFree(Polynomial<Coef, Order> f) {
    using Poly = Polynomial<Coef, Order>;

    std::vector<Poly> result;

    if (f.degree() == 1) {
        result.push_back(f);
        return result;
    }

    // 適切な素数の選択
    int p = findGoodPrime(f);

    // mod p
    auto fp = modPolynomial(f, p);

    // 有限体因数分解
    auto modFactors = berlekampFactor(fp, p);

    // Hensel lifting
    auto tree = buildFactorTree(modFactors, 0, modFactors.size());
    henselLiftTree(f, *tree, 0, 0);

    // 因子を取得
    collectLeaves(*tree, result);

    return result;
}
template <class Coef, class Order>
std::vector<std::pair<Polynomial<Coef, Order>, int64_t>> factorize(Polynomial<Coef, Order> f) {
    using Poly = Polynomial<Coef, Order>;

    std::vector<std::pair<Poly, int64_t>> ans;

    // 定数なら終了
    if (f.isConstant()) return ans;

    // 1. content 除去
    Coef c = content(f);
    f *= Coef(1) / c;

    // 2. square-free decomposition
    auto sqfs = squareFreeDecomposition(f);

    // 3. 各 square-free 部分を因数分解
    for (auto& sqf : sqfs) {
        auto factors = factorSquareFree(sqf.poly);

        for (auto& h : factors) {
            ans.push_back({h, sqf.multiplicity});
        }
    }

    return ans;
}

}  // namespace sci_calc
