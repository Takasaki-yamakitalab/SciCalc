#ifndef SCI_CALC_DEFINE_HPP
#define SCI_CALC_DEFINE_HPP

#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <complex>
#include <map>
#include <cassert>

namespace sci_calc {
enum OPE_TYPE { OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_POW, OP_END };
enum NODE_TYPE { ND_NUM, ND_OPE, ND_VAR, ND_MATH_FUNC, ND_SPECIAL_FUNC, ND_ASSIGN, ND_END };

const char OP_LIST[OP_END][2] = {"+", "-", "*", "/", "^"};
constexpr int DEFAULT_INTEGRAL_N = 0x100000;

constexpr uint32_t OPTIMIZER_NONE = 0;
constexpr uint32_t OPTIMIZER_CONST_FOLDING = 1 << 0;
constexpr uint32_t OPTIMIZER_CONST_PROPAGATION = 1 << 1;
constexpr uint32_t OPTIMIZER_ALGEBRAIC_SIMPLIFICATION = 1 << 2;
constexpr uint32_t OPTIMIZER_COMBINE_LIKE_TERMS = 1 << 3;

constexpr uint32_t OPTIMIZER_ALL = 0xFFFFFFFF;

constexpr uint32_t OPTIMIZER_DEFAULT = OPTIMIZER_CONST_FOLDING + OPTIMIZER_CONST_PROPAGATION +
                                       OPTIMIZER_ALGEBRAIC_SIMPLIFICATION + OPTIMIZER_COMBINE_LIKE_TERMS;

using Complex = std::complex<double>;

struct Node {
    size_t hash;
    NODE_TYPE type = ND_NUM;
    int id = 0;
    Complex value = 0.0;
    std::vector<std::unique_ptr<Node>> children;
    Node();
    Node(const Node& other);
    Node(Complex val);
    ~Node() noexcept {}
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;
};
using NodePtr = std::unique_ptr<Node>;

struct Memory {
    std::string name;
    Complex value;
    Memory(std::string name, Complex v);
};

struct MathFunc {
    std::string name;
    std::function<Complex(Complex)> calc;
    MathFunc(std::string name, std::function<Complex(Complex)> f);
};

struct SpecialFunc {
    std::string name;
    int arg_num;
    std::function<std::complex<double>(const std::vector<NodePtr>&)> calc;
    SpecialFunc(std::string name, int argc, std::function<Complex(const std::vector<NodePtr>&)> f);
};

struct Monomial {
    std::vector<int> exp;
    bool operator<(const Monomial& mono) const {
        assert(exp.size() == mono.exp.size());
        for (size_t i = 0; i < exp.size(); i++) {
            if (exp[i] != mono.exp[i]) return exp[i] > mono.exp[i];
        }
        return true;
    }
};

template <class Coef>
struct Polynomial {
    std::map<Monomial, Coef> terms;
};
}  // namespace sci_calc

#endif
