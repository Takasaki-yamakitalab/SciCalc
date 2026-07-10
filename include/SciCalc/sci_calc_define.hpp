#ifndef SCI_CALC_DEFINE_HPP
#define SCI_CALC_DEFINE_HPP

#include <stdio.h>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace sci_calc {
enum OPE_TYPE {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_END
};
enum NODE_TYPE {
    ND_NUM,
    ND_OPE,
    ND_VAR,
    ND_MATH_FUNC,
    ND_SPECIAL_FUNC,
    ND_ASSIGN,
    ND_END
};

const char OP_LIST[OP_END][2] = {"+", "-", "*", "/", "^"};
constexpr int DEFAULT_INTEGRAL_N = 0x100000;

constexpr uint32_t OPTIMIZER_NONE = 0;
constexpr uint32_t OPTIMIZER_CONST_FOLDING = 1 << 0;
constexpr uint32_t OPTIMIZER_CONST_PROPAGATION = 1 << 1;
constexpr uint32_t OPTIMIZER_ALGEBRAIC_SIMPLIFICATION = 1 << 2;
constexpr uint32_t OPTIMIZER_COMBINE_LIKE_TERMS = 1 << 3;

constexpr uint32_t OPTIMIZER_ALL = 0xFFFFFFFF;

constexpr uint32_t OPTIMIZER_DEFAULT = OPTIMIZER_CONST_FOLDING + OPTIMIZER_CONST_PROPAGATION + OPTIMIZER_ALGEBRAIC_SIMPLIFICATION + OPTIMIZER_COMBINE_LIKE_TERMS;

struct Node {
    size_t hash;
    NODE_TYPE type = ND_NUM;
    int id = 0;
    double value = 0.0;
    std::vector<std::unique_ptr<Node>> children;
    Node();
    Node(const Node& other);
    Node(double val);
    ~Node() noexcept {}
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;
};
using NodePtr = std::unique_ptr<Node>;

struct Memory {
    std::string name;
    double value;
    Memory(std::string name, double v);
};

struct MathFunc {
    std::string name;
    std::function<double(double)> calc;
    MathFunc(std::string name, std::function<double(double)> f);
};

struct SpecialFunc {
    std::string name;
    int arg_num;
    std::function<double(const std::vector<NodePtr>&)> calc;
    SpecialFunc(std::string name, int argc, std::function<double(const std::vector<NodePtr>&)> f);
};

}  // namespace sci_calc

#endif
