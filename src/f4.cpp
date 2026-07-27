#include "SciCalc/f4mat.hpp"
#include <set>
#include <deque>
#include <utility>

namespace sci_calc {

template <class Coef>
int SparseRow<Coef>::leadingColumn() const {
    assert(!coef.empty());
    return coef.rbegin()->first;
}
template <class Coef>
Coef SparseRow<Coef>::leadingCoefficient() const {
    assert(!coef.empty());
    return coef.rbegin()->second;
}
template <class Coef>
void SparseRow<Coef>::normalize() {
    for (auto it = coef.begin(); it != coef.end();) {
        if (it->second == Coef(0))
            it = coef.erase(it);
        else
            it++;
    }
}
template <class Coef>
void SparseRow<Coef>::axpy(Coef a, const SparseRow& r) {
    if (a == Coef(0)) return;
    for (auto it = r.coef.begin(); it != r.coef.end(); it++) {
        auto dst = coef.find(it->first);
        if (dst == coef.end()) {
            Coef v = a * it->second;
            if (v != Coef(0)) coef.insert(std::make_pair(it->first, v));
        } else {
            dst->second += a * it->second;
            if (dst->second == Coef(0)) coef.erase(dst);
        }
    }
}

template <class Coef, class Order>
void F4Matrix<Coef, Order>::buildColumns(const std::vector<Polynomial<Coef, Order>>& P) {
    std::set<Monomial, Order> mons;
    for (auto& p : P) {
        for (auto& t : p.terms) {
            mons.insert(t.first);
        }
    }
    columns.assign(mons.begin(), mons.end());
    for (int i = 0; i < columns.size(); i++) {
        columnIndex[columns[i]] = i;
    }
}
template <class Coef, class Order>
void F4Matrix<Coef, Order>::addReducer(const Polynomial<Coef, Order>& p) {
    SparseRow<Coef> row;
    for (auto& term : p.terms) {
        int col = columnIndex[term.first];
        row[col] = term.second;
    }
    reducers.push_back(row);
}
template <class Coef, class Order>
void F4Matrix<Coef, Order>::addTarget(const Polynomial<Coef, Order>& p) {
    SparseRow<Coef> row;
    for (auto& term : p.terms) {
        int col = columnIndex[term.first];
        row[col] = term.second;
    }
    targets.push_back(row);
}
template <class Coef, class Order>
Polynomial<Coef, Order> F4Matrix<Coef, Order>::row2Polynomial(const SparseRow<Coef>& row) const {
    Polynomial<Coef, Order> ret;
    for (auto it = row.coef.begin(); it != row.coef.end(); it++) {
        ret.addTerm(columns[it->first], it->second);
    }
    return ret;
}
template <class Coef, class Order>
void F4Matrix<Coef, Order>::symbolicPreprocessing(const std::vector<Polynomial<Coef, Order>>& G) {
    std::set<Monomial, Order> todo;
    std::set<Monomial, Order> visited;
    std::set<std::pair<int, Monomial>> usedReducers;

    for (auto& p : targets) {
        for (auto it = p.coef.begin(); it != p.coef.end(); it++) {
            todo.insert(columns[it->first]);
        }
    }
    while (!todo.empty()) {
        Monomial m = *todo.rbegin();
        todo.erase(std::prev(todo.end()));
        if (visited.count(m)) continue;
        visited.insert(m);

        int g = findReducer(G, m);
        if (g == -1) continue;

        Monomial u = m / G[g].leadingMonomial();
        if (!usedReducers.insert({g, u}).second) continue;

        Polynomial<Coef, Order> p = G[g] * Term<Coef>{u, Coef(1)};

        addReducer(p);

        for (auto it = p.terms.begin(); it != p.terms.end(); it++) {
            if (!visited.count(it->first)) todo.insert(it->first);
        }
    }
}

template <class Coef, class Order>
void triangularizeReducers(F4Matrix<Coef, Order>& M) {
    std::map<int, int> pivot;
    for (int i = 0; i < M.reducers.size(); i++) {
        auto& row = M.reducers[i];
        while (!row.coef.empty()) {
            int col = row.leadingColumn();
            auto it = pivot.find(col);
            if (it == pivot.end()) {
                pivot[col] = i;
                break;
            }

            auto& piv = M.reducers[it->second];
            Coef c = row.leadingCoefficient() / piv.leadingCoefficient();
            row.axpy(-c, piv);
        }
    }
}
template <class Coef, class Order>
std::vector<SparseRow<Coef>> reduceTargets(F4Matrix<Coef, Order>& M) {
    std::map<int, int> pivot;
    for (int i = 0; i < M.reducers.size(); i++) {
        pivot[M.reducers[i].leadingColumn()] = i;
    }

    std::vector<SparseRow<Coef>> newRows;
    for (auto row : M.targets) {
        while (!row.coef.empty()) {
            int col = row.leadingColumn();
            auto it = pivot.find(col);
            if (it == pivot.end()) break;

            auto& piv = M.reducers[it->second];
            Coef c = row.leadingCoefficient() / piv.leadingCoefficient();
            row.axpy(-c, piv);
        }

        row.normalize();
        if (!row.coef.empty()) newRows.push_back(row);
    }
    return newRows;
}
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> f4ReduceBatch(const std::vector<Polynomial<Coef, Order>>& G,
                                                   const std::vector<Polynomial<Coef, Order>>& S) {
    using Poly = Polynomial<Coef, Order>;
    F4Matrix<Coef, Order> M;

    std::vector<Poly> tmp = S;
    tmp.insert(tmp.end(), G.begin(), G.end());
    M.buildColumns(tmp);

    for (size_t i = 0; i < S.size(); i++) {
        M.addTarget(S[i]);
    }

    M.symbolicPreprocessing(G);
    M.triangularizeReducers();
    std::vector<SparseRow<Coef>> rows = reduceTargets(M);

    std::vector<Poly> ret;
    for (size_t i = 0; i < rows.size(); i++) {
        Poly p = M.row2Polynomial(rows[i]);
        if (!p.isZero()) ret.push_back(p);
    }
    return ret;
}
template <class Coef, class Order>
std::vector<Polynomial<Coef, Order>> f4(std::vector<Polynomial<Coef, Order>> G) {
    using Poly = Polynomial<Coef, Order>;
    std::deque<std::pair<int, int>> pairs;
    for (int i = 0; i < (int)G.size(); i++) {
        for (int j = i + 1; j < (int)G.size(); j++) {
            pairs.push_back(std::make_pair(i, j));
        }
    }

    while (!pairs.empty()) {
        std::vector<Poly> batch;
        const int BatchSize = 32;
        while (!pairs.empty() && batch.size() < BatchSize) {
            int i = pairs.front().first;
            int j = pairs.front().second;
            pairs.pop_front();

            if (gcd(G[i].leadingMonomial(), G[j].leadingMonomial()).isOne()) continue;

            batch.push_back(sPolynomial(G[i], G[j]));
        }

        std::vector<Poly> newPolys = f4ReduceBatch(G, batch);

        for (size_t i = 0; i < newPolys; i++) {
            bool exist = false;
            for (size_t j = 0; j < G.size(); j++) {
                if (newPolys[i] == G[j]) {
                    exist = true;
                    break;
                }
            }

            if (exist) continue;

            int k = (int)G.size();
            G.push_back(newPolys[i]);
            for (int t = 0; t < k; t++) {
                pairs.push_back(std::make_pair(t, k));
            }
        }
    }
    return reduceGroebnerBasis(G);
}

}  // namespace sci_calc
