#include "SciCalc/sci_calc_define.hpp"
#include <memory>

namespace sci_calc {
Node::Node() {}

Node::Node(const Node& other)
    : hash(other.hash), type(other.type), id(other.id), value(other.value) {
    for (size_t i = 0; i < other.children.size(); i++) {
        children.push_back(std::make_unique<Node>(*other.children[i]));
    }
}

Node::Node(Complex val) : type(ND_NUM), value(val) {}

Memory::Memory(std::string n, Complex v) : name(n), value(v) {}

MathFunc::MathFunc(std::string n, std::function<Complex(Complex)> f)
    : name(n), calc(f) {}

SpecialFunc::SpecialFunc(std::string n, int argc, const std::vector<int>& index, std::function<Complex(const std::vector<NodePtr>&)> f)
    : name(n), arg_num(argc), var_index(index), calc(f) {}

OriginalFunc::OriginalFunc(const std::string& n, const std::vector<int>& var_index, const NodePtr& node) : name(n), var_id(var_index) {
    content = std::make_unique<Node>(*node);
}
}  // namespace sci_calc
