#ifndef MODINT_HPP
#define MODINT_HPP

#include <ostream>
#include "coefficient_traits.hpp"

namespace sci_calc {

template <int P>
class ModInt {
    int v;

   public:
    ModInt(int x = 0) {
        v = x % P;
        if (v < 0) v += P;
    }

    int value() const { return v; }
    bool isZero() const { return v == 0; }
    bool isOne() const { return v == 1; }
    static int modPow(int a, int64_t e) {
        int64_t r = 1;
        int64_t x = a;
        while (e > 0) {
            if (e & 1) r = r * x % P;
            x = x * x % P;
            e >>= 1;
        }
        return (int)r;
    }
    ModInt inverse() const {
        assert(v != 0);
        return ModInt(modPow(v, P - 2));
    }

    ModInt operator-() const { return ModInt(-v); }

    ModInt operator+(const ModInt& b) const { return ModInt(v + b.v); }
    ModInt operator-(const ModInt& b) const { return ModInt(v - b.v); }
    ModInt operator*(const ModInt& b) const { return ModInt((int64_t)v * b.v); }
    ModInt operator/(const ModInt& b) const { return *this * b.inverse(); }

    friend ModInt operator+(const int& a, const ModInt& b) { return ModInt(a) + b; }
    friend ModInt operator-(const int& a, const ModInt& b) { return ModInt(a) - b; }
    friend ModInt operator*(const int& a, const ModInt& b) { return ModInt(a) * b; }
    friend ModInt operator/(const int& a, const ModInt& b) { return ModInt(a) / b; }

    ModInt& operator+=(const ModInt& b) {
        *this = *this + b;
        return *this;
    }
    ModInt& operator-=(const ModInt& b) {
        *this = *this - b;
        return *this;
    }
    ModInt& operator*=(const ModInt& b) {
        *this = *this * b;
        return *this;
    }
    ModInt& operator/=(const ModInt& b) {
        *this = *this / b;
        return *this;
    }

    bool operator==(const ModInt& b) const { return v == b.v; }
    bool operator!=(const ModInt& b) const { return v != b.v; }

    friend std::ostream& operator<<(std::ostream& os, const ModInt& a) { return os << a.v; }
};

template <int P>
struct coefficient_traits<ModInt<P>> {
    static const bool is_field = true;
};

}  // namespace sci_calc

#endif
