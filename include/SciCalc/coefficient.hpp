#ifndef COEFFICIENT_HPP
#define COEFFICIENT_HPP

#include <complex>
#include <vector>

namespace sci_calc {

template <class Coef>
Coef content(const std::vector<Coef>& c);
template <class Coef>
std::complex<Coef> content(const std::vector<std::complex<Coef>>& c);

int64_t permutation(const int64_t& n, const int64_t& r);
int64_t combination(const int64_t& n, const int64_t& r);

template <class Coef>
Coef pow(Coef c, int64_t n);

}

#endif
