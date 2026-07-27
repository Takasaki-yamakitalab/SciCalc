#include "SciCalc/term.hpp"

namespace sci_calc {

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

}
