#ifndef TERM_HPP
#define TERM_HPP

#include "monomial.hpp"

namespace sci_calc {

template <class Coef>
struct Term {
    Monomial mono;
    Coef coef;

    bool isZero() const;

    bool operator==(const Term<Coef>& t) const;
    bool operator!=(const Term<Coef>& t) const;
    Term<Coef> operator-() const;
};

template <class Coef>
Term<Coef> operator*(const Term<Coef>& a, const Term<Coef>& b);
template <class Coef>
Term<Coef> operator/(const Term<Coef>& a, const Term<Coef>& b);
template <class Coef>
Term<Coef> operator*(Term<Coef> t, const Coef& c);
template <class Coef>
Term<Coef> operator*(const Coef& c, Term<Coef> t);

}

#endif
