#include "SciCalc/coefficient.hpp"

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
}
