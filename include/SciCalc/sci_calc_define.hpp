#ifndef SCI_CALC_DEFINE_HPP
#define SCI_CALC_DEFINE_HPP

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
        ND_VAR,
        ND_MATH_FUNC,
        ND_SPECIAL_FUNC,
        ND_OPE,
        ND_ASSIGN,
        ND_END
    };

    const char OP_LIST[OP_END][2] = { "+", "-", "*", "/", "^" };
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
    };
    using NodePtr = std::unique_ptr<Node>;

    struct Memory {
        std::string name;
        double value;
        Memory(std::string n, double v) : name(n), value(v) {}
    };

    struct MathFunc {
        std::string name;
        std::function<double(double)> calc;
        MathFunc(std::string n, std::function<double(double)> f) : name(n), calc(f) {}
    };

    struct SpecialFunc {
        std::string name;
        int arg_num;
        std::function<double(const std::vector<NodePtr>&)> calc;
        SpecialFunc(std::string n, int argc, std::function<double(const std::vector<NodePtr>&)> f) : name(n), arg_num(argc), calc(f) {}
    };
}

#endif
