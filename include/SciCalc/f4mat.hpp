#ifndef F4MAT_HPP
#define F4MAT_HPP

#include "polynomial.hpp"

namespace sci_calc {

template <class Coef>
struct SparseRow {
    std::map<int, Coef> coef;

    int leadingColumn() const;
    Coef leadingCoefficient() const;
    void normalize();
    void axpy(Coef a, const SparseRow& r);

    Coef& operator[](int key) { return coef[key]; }
};

template <class Coef, class Order>
struct F4Matrix {
    std::vector<Monomial> columns;
    std::map<Monomial, int, Order> columnIndex;
    std::vector<SparseRow<Coef>> reducers;      // 既存Groebner基底由来
    std::vector<SparseRow<Coef>> targets;       // S多項式由来

    void buildColumns(const std::vector<Polynomial<Coef, Order>>& polys);
    void addReducer(const Polynomial<Coef, Order>& p);
    void addTarget(const Polynomial<Coef, Order>& p);
    Polynomial<Coef, Order> row2Polynomial(const SparseRow<Coef>& row) const;
    void symbolicPreprocessing(const std::vector<Polynomial<Coef, Order>>& G);

    void triangularizeReducers() { triangularizeReducers(*this); }
};

template <class Coef, class Order>
void triangularizeReducers(F4Matrix<Coef, Order>& M);
template <class Coef, class Order>
std::vector<SparseRow<Coef>> reduceTargets(F4Matrix<Coef, Order>& M);

template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> f4ReduceBatch(const std::vector<Polynomial<Coef, Order>>& G, const std::vector<Polynomial<Coef, Order>>& S);
}  // namespace sci_calc

#endif
