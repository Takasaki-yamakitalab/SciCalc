#include "SciCalc/sci_calc.hpp"
#include "SciCalc/sci_calc_define.hpp"
#include <cmath>

namespace sci_calc {
int SciCalc::set_integral_N(int N) {
    return this->m_integral_N = N;
}

int SciCalc::set_special_func_list() {
    m_special_functions.clear();
    // special functions
    // sort by longest name first
    // all special function names begin with '\\'
    // the content of var_id represents the index within the '{}' structure used to store variables.
    m_special_functions.emplace_back("\\defunc", -1, std::vector<int>{ -1, 0, -1 }, [&](const std::vector<NodePtr>& children) { return evaluate_defunc_node(children); });
    m_special_functions.emplace_back("\\cfunc", -1, std::vector<int>{}, [&](const std::vector<NodePtr>& children) { return evaluate_cfunc_node(children); });
    m_special_functions.emplace_back("\\prod", 4, std::vector<int>{ 1 }, [&](const std::vector<NodePtr>& children) { return evaluate_prod_node(children); });
    m_special_functions.emplace_back("\\diff", 3, std::vector<int>{ 1 }, [&](const std::vector<NodePtr>& children) { return evaluate_diff_node(children); });
    m_special_functions.emplace_back("\\perm", 2, std::vector<int>{}, [&](const std::vector<NodePtr>& children) { return evaluate_perm_node(children); });
    m_special_functions.emplace_back("\\comb", 2, std::vector<int>{}, [&](const std::vector<NodePtr>& children) { return evaluate_comb_node(children); });
    m_special_functions.emplace_back("\\sum", 4, std::vector<int>{ 1 }, [&](const std::vector<NodePtr>& children) { return evaluate_sum_node(children); });
    m_special_functions.emplace_back("\\int", 4, std::vector<int>{ 1 }, [&](const std::vector<NodePtr>& children) { return evaluate_int_node(children); });
    m_special_functions.emplace_back("\\log", 2, std::vector<int>{}, [&](const std::vector<NodePtr>& children) { return evaluate_log_node(children); });
    m_special_functions.emplace_back("\\abs", 1, std::vector<int>{}, [&](const std::vector<NodePtr>& children) { return evaluate_abs_node(children); });

    // error
    m_special_functions.emplace_back("", 4, std::vector<int>{}, [](const std::vector<NodePtr>& children) {
        (void)children;
        fprintf(stderr, "[ERROR] Undefined function name.\n");
        return NAN;
    });
    return 0;
}
int SciCalc::add_special_func_to_list(const SpecialFunc& func) {
    for (int i = 0; i <= m_special_functions.size(); i++) {
        if (func.name.size() < m_special_functions[i].name.size()) {
            m_special_functions.insert(m_special_functions.begin() + i, func);
            return 0;
        }
    }
    m_special_functions.push_back(func);
    return 0;
}

int SciCalc::detect_special_func(const std::string& func) {
    int id = 0;
    for (id = 0; id < m_special_functions.size(); id++) {
        if (func.find(m_special_functions[id].name) == 0) return id;
    }
    return (int)m_special_functions.size() - 1;
}

std::string SciCalc::rm_special_func_name(std::string& func) {
    int id = detect_special_func(func);
    size_t nameLen = m_special_functions[id].name.size();
    if (nameLen == 0) return func.erase(0);
    return func.erase(0, nameLen);
}

std::vector<std::string> SciCalc::get_special_func_args(const std::string& func) {
    std::vector<std::string> args;
    size_t start_index = 1;
    int brackets = 0;
    for (size_t i = 0; i < func.size(); i++) {
        brackets += is_bracket(func[i]);
        if (brackets == 0 && start_index < i) {
            args.push_back(func.substr(start_index, i - start_index));
            start_index = i + 2;
        }
    }
    return args;
}

Complex SciCalc::evaluate_sum_node(const std::vector<NodePtr>& children) {
    if (children.size() != 4) return NAN;
    Complex sum = 0.0;
    int var_id = children[1]->id;
    int start = (int)(evaluate_node(children[2]).real() + 0.5);
    int end = (int)(evaluate_node(children[3]).real() + 0.5);
    Complex memory = m_memory[var_id].value;
    for (int i = start; i <= end; i++) {
        m_memory[var_id].value = i;
        sum += evaluate_node(children[0]);
    }
    m_memory[var_id].value = memory;
    return sum;
}
Complex SciCalc::evaluate_prod_node(const std::vector<NodePtr>& children) {
    if (children.size() != 4) return NAN;
    Complex prod = 1.0;
    int var_id = children[1]->id;
    int start = (int)(evaluate_node(children[2]).real() + 0.5);
    int end = (int)(evaluate_node(children[3]).real() + 0.5);
    Complex memory = m_memory[var_id].value;
    for (int i = start; i <= end; i++) {
        m_memory[var_id].value = i;
        prod *= evaluate_node(children[0]);
    }
    m_memory[var_id].value = memory;
    return prod;
}
Complex SciCalc::evaluate_int_node(const std::vector<NodePtr>& children) {
    if (children.size() != 4) return NAN;
    int var_id = children[1]->id;
    double start = evaluate_node(children[2]).real();
    double end = evaluate_node(children[3]).real();
    int sign = 1;
    if (start > end) {
        std::swap(start, end);
        sign = -1;
    }
    double dx = (end - start) / this->m_integral_N;
    Complex ans = 0.0;
    Complex memory = m_memory[var_id].value;
    m_memory[var_id].value = start;
    ans += evaluate_node(children[0]);
    m_memory[var_id].value = end;
    ans += evaluate_node(children[0]);
    ans /= 2.0;
    for (double x = start + dx; x < end; x += dx) {
        m_memory[var_id].value = x;
        ans += evaluate_node(children[0]);
    }
    ans *= dx;
    m_memory[var_id].value = memory;
    return ans * (double)sign;
}
Complex SciCalc::evaluate_diff_node(const std::vector<NodePtr>& children) {
    if (children.size() != 3) return NAN;
    int var_id = children[1]->id;
    Complex x = evaluate_node(children[2]);
    double eps = std::numeric_limits<double>::epsilon();
    double h = std::sqrt(eps) * std::max(std::abs(x), 1.0);
    Complex ans = 0.0;
    Complex memory = m_memory[var_id].value;
    m_memory[var_id].value = x + h;
    ans = evaluate_node(children[0]);
    m_memory[var_id].value = x - h;
    ans -= evaluate_node(children[0]);
    ans /= 2 * h;
    m_memory[var_id].value = memory;
    return ans;
}
Complex SciCalc::evaluate_perm_node(const std::vector<NodePtr>& children) {
    if (children.size() != 2) return NAN;
    return permutation(evaluate_node(children[0]).real(), evaluate_node(children[1]).real());
}
Complex SciCalc::evaluate_comb_node(const std::vector<NodePtr>& children) {
    if (children.size() != 2) return NAN;
    return combination(evaluate_node(children[0]).real(), evaluate_node(children[1]).real());
}
Complex SciCalc::evaluate_log_node(const std::vector<NodePtr>& children) {
    if (children.size() != 2) return NAN;
    return log(evaluate_node(children[1])) / log(evaluate_node(children[0]));
}
Complex SciCalc::evaluate_defunc_node(const std::vector<NodePtr> &children) {
    if (children.size() < 3) return NAN;
    int func_id = children[0]->id;
    int vars_num = (int)children.size() - 2;
    std::vector<int> var_id;
    for (int i = 0; i < vars_num; i++) {
        var_id.push_back(children[1 + i]->id);
    }
    m_original_functions[func_id] = OriginalFunc(m_memory[func_id].name, var_id, children.back());
    return Complex(0.0);
}
Complex SciCalc::evaluate_cfunc_node(const std::vector<NodePtr> &children) {
    if (children.size() < 2) return NAN;
    int func_id = children[0]->id;

    auto f = m_original_functions.find(func_id);
    if (f == m_original_functions.end()) return NAN;

    if (f->second.var_id.size() != children.size() - 1) return NAN;
    std::vector<Complex> memory;
    for (int i = 0; i < f->second.var_id.size(); i++) {
        memory.push_back(m_memory[f->second.var_id[i]].value);
        m_memory[f->second.var_id[i]].value = evaluate_node(children[1 + i]);
    }
    Complex ans = evaluate_node(f->second.content);
    for (int i = 0; i < f->second.var_id.size(); i++) {
        m_memory[f->second.var_id[i]].value = memory[i];
    }

    return ans;
}
Complex SciCalc::evaluate_abs_node(const std::vector<NodePtr> &children) {
    if (children.size() != 1) return NAN;
    Complex eval = evaluate_node(children[0]);
    return sqrt(eval.real() * eval.real() + eval.imag() * eval.imag());
}
}  // namespace sci_calc
