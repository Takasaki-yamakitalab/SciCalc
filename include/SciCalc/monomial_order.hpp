#ifndef MONOMIAL_ORDER_HPP
#define MONOMIAL_ORDER_HPP

#include "monomial.hpp"

namespace sci_calc {

struct LexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};
struct GrLexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};
struct GrevLexOrder {
    bool operator()(const Monomial& a, const Monomial& b) const;
};


}

#endif
