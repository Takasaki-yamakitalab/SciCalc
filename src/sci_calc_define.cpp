#include "SciCalc/sci_calc_define.hpp"

namespace sci_calc {
Node::Node() {}

Node::Node(const Node& other)
    : hash(other.hash), type(other.type), id(other.id), value(other.value) {
    for (size_t i = 0; i < other.children.size(); i++) {
        children.push_back(std::make_unique<Node>(*other.children[i]));
    }
}

Node::Node(double val) : type(ND_NUM), value(val) {}

Memory::Memory(std::string n, double v) : name(n), value(v) {}

MathFunc::MathFunc(std::string n, std::function<double(double)> f)
    : name(n), calc(f) {}

SpecialFunc::SpecialFunc(std::string n, int argc,
                         std::function<double(const std::vector<NodePtr>&)> f)
    : name(n), arg_num(argc), calc(f) {}
}  // namespace sci_calc
