#ifndef SCI_CALC_DEFINE_HPP
#define SCI_CALC_DEFINE_HPP

#include <stdio.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <complex>
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

using Real = double;
using Complex = std::complex<Real>;

struct Node;
using NodePtr = std::unique_ptr<Node>;
struct Node {
    size_t hash;
    NODE_TYPE type = ND_NUM;
    int id = 0;
    Complex value = 0.0;
    std::vector<NodePtr> children;
    Node();
    Node(const Node& other);
    Node(Complex val);
    ~Node() noexcept {}
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;
};

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
    int arg_num;    // -1 for variable-length arguments

    // The argument indicated by var_index is treated as a variable
    // and will not be folded during the optimization.
    // However, if var_index[0] is -1, then any argument not included
    // in var_index is treated as a variable.
    // For example, if you wish to treat all arguments except the
    // first (index 0) and the last as variables, you would set
    // `var_index = {-1, 0, -1}`.
    std::vector<int> var_index;
    std::function<std::complex<double>(const std::vector<NodePtr>&)> calc;
    SpecialFunc(std::string name, int argc, const std::vector<int>& index, std::function<Complex(const std::vector<NodePtr>&)> f);
};

struct OriginalFunc {
    std::string name = "";
    std::vector<int> var_id = {};
    NodePtr content = nullptr;
    OriginalFunc() = default;
    OriginalFunc(const std::string& name, const std::vector<int>& var_id, const NodePtr& node);
};

}  // namespace sci_calc

#endif
