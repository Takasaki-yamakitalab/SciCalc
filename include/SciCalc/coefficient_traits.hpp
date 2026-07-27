#ifndef COEFFICIENT_TRAITS_HPP
#define COEFFICIENT_TRAITS_HPP

namespace sci_calc {

template <class Coef>
struct coefficient_traits {
    static const bool is_field = false;
};

template <>
struct coefficient_traits<double> {
    static const bool is_field = true;
};

template <>
struct coefficient_traits<int> {
    static const bool is_field = false;
};
}
#endif
