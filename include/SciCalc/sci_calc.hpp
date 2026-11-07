#ifndef SCI_CALC_HPP
#define SCI_CALC_HPP

#include <Polynomial/monomial_order.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <Polynomial/polynomial.hpp>
#include "./sci_calc_define.hpp"

namespace sci_calc {

// sci_calc_csv.cpp
std::vector<std::vector<std::string>> read_csv(const std::string& fname);
int write_csv(const std::string& fname, const std::vector<std::vector<std::string>>& dat);

// sci_calc_tools.cpp
int64_t get_raw(double x);
double raw2double(int64_t raw);
void saveNode(std::ostream& os, const Node& node);
NodePtr loadNode(std::istream& is);
void saveOriginalFunctions(std::ostream& os, const std::unordered_map<int, OriginalFunc>& func);
void saveOriginalFunc(std::ostream& os, const OriginalFunc& func);
std::unordered_map<int, OriginalFunc> loadOriginalFunctions(std::istream& is);
OriginalFunc loadOriginalFunc(std::istream& is);
void saveMemory(std::ostream& os, const Memory& mem);
void saveMemories(std::ostream& os, const std::vector<Memory>& mems);
Memory loadMemory(std::istream& is);
std::vector<Memory> loadMemories(std::istream& is);
void formatMemories(std::vector<Memory>& mems);

class SciCalc {
   private:
    bool is_debug;
    int m_integral_N = DEFAULT_INTEGRAL_N;
    std::string m_path;

    // list of the variables (memory)
    std::vector<Memory> m_memory;

    // list of the mathematical functions
    // sort by longest name first
    std::vector<MathFunc> m_functions;

    // list of the special functions
    // sort by longest name first
    std::vector<SpecialFunc> m_special_functions;

    // list for user's original functions
    // key: function's id (index within m_memory)
    std::unordered_map<int, OriginalFunc> m_original_functions;

    // list of variables that are treated as "variables"
    // variables not registered in this list will be expanded to constants
    std::vector<int> m_var_id_stack;

    bool list_all_var = false;

    uint32_t m_optimizer = OPTIMIZER_DEFAULT;

    NodePtr ast_cash = NULL;

   public:
    // sci_calc.cpp
    SciCalc();
    SciCalc(const std::string& path);
    SciCalc(int N, const std::string& path);
    SciCalc(bool debug, const std::string& path);
    SciCalc(int N, bool debug, const std::string& path);
    ~SciCalc();
    int init();
    std::string rm_spaces(const std::string& src);
    Complex calc(const std::string& formula);
    std::string expand(const std::string& formula);
    std::string factorize(const std::string& formula);

    // sci_calc_ast.cpp
    int is_bracket(char c);
    std::string rm_brackets_once(std::string& src);
    std::string rm_brackets(std::string& src);
    std::string split_by_ope(std::string& formula, const char ope);
    std::string split_by_ope(std::string& formula, const char* ope);
    static size_t node_hash(const Node* node);
    NodePtr parse(std::string& formula);
    int parse_special_func_args(std::string& func, NodePtr& node);
    int parse_numbers(std::string& formula, NodePtr& node);
    Complex evaluate_node(const NodePtr& node);

    // sci_calc_optimize.cpp
    bool is_equal(const Node* a, const Node* b);
    uint32_t change_optimizer(const std::string& opt = "default");
    uint32_t add_optimizer(const std::string& opt);
    int optimize_node(NodePtr& node);
    int optimizer_const_folding(NodePtr& node);
    int optimizer_const_propagation(NodePtr& node);
    int optimizer_algebraic_simplification(NodePtr& node);
    int optimizer_combine_like_terms(NodePtr& node);
    int optimize_flatten_add(NodePtr& node);
    int optimize_flatten_mul(NodePtr& node);
    int optimize_sub(NodePtr& node);
    int optimize_div(NodePtr& node);
    int optimize_pow(NodePtr& node);

    // sci_calc_mathfunc.cpp
    int set_math_func_list();
    int add_math_func_to_list(const MathFunc& func);
    bool ends_with_math_func(const std::string& str);
    int detect_func(const std::string& func);
    std::string rm_func_name(std::string& func);
    int permutation(double n, double r);
    int combination(double n, double r);
    int permutation(int n, int r);
    int combination(int n, int r);

    // sci_calc_memory.cpp
    int set_memory(const std::string& path);
    int save_memory(const std::string& path);
    int reset_default_mems();
    int get_memory_id(const std::string& name);
    Complex get_memory(const std::string& name);
    Complex assign_memory(const std::string& name, Complex val);
    void load_memories(const std::vector<Memory>& mems);
    std::vector<Memory> get_all_memories();
    int list_var_id(int id);

    // sci_calc_special_functions.cpp
    int set_integral_N(int N = DEFAULT_INTEGRAL_N);
    int set_special_func_list();
    int add_special_func_to_list(const SpecialFunc& func);
    int detect_special_func(const std::string& func);
    std::string rm_special_func_name(std::string& func);
    std::vector<std::string> get_special_func_args(const std::string& func);
    Complex evaluate_sum_node(const std::vector<NodePtr>& children);
    Complex evaluate_prod_node(const std::vector<NodePtr>& children);
    Complex evaluate_int_node(const std::vector<NodePtr>& children);
    Complex evaluate_diff_node(const std::vector<NodePtr>& children);
    Complex evaluate_perm_node(const std::vector<NodePtr>& children);
    Complex evaluate_comb_node(const std::vector<NodePtr>& children);
    Complex evaluate_log_node(const std::vector<NodePtr>& children);
    Complex evaluate_defunc_node(const std::vector<NodePtr>& children);
    Complex evaluate_cfunc_node(const std::vector<NodePtr>& children);
    Complex evaluate_abs_node(const std::vector<NodePtr>& children);

    // sci_calc_expand.cpp
    int expand(NodePtr& node);
    int flatten_add(NodePtr& node);
    int flatten_mul(NodePtr& node);
    int expand_pow(NodePtr& node);
    int cartesian_prod(const std::vector<std::vector<NodePtr>>& sets, std::vector<std::vector<NodePtr>>& dst,
                       std::vector<NodePtr>& cur, size_t depth);

    // sci_calc_to_string.cpp
    std::string to_string(const NodePtr& node);
    std::string number_to_string(const NodePtr& node);
    std::string special_func_to_string(const NodePtr& node);
    bool need_paren(const NodePtr& node, const NodePtr& child);
    std::string to_string_g(const double value, const int precision = 12);
    std::string to_string_g(const Complex value, const int precision = 12);
    std::string to_string_debug(const NodePtr& node);

    // sci_calc_sort.cpp
    int sort_children(NodePtr& node);
    int sort_flatten_add(NodePtr& node);
    int sort_flatten_mul(NodePtr& node);

    // sci_calc_polynomial.cpp
    polynomial::Monomial make_monomial(const NodePtr& node);
    polynomial::Polynomial<int64_t, polynomial::LexOrder> make_polynomial(const NodePtr& node);
    NodePtr poly2node(const polynomial::Polynomial<int64_t, polynomial::LexOrder>& poly);
    void mono2node(NodePtr& dst, const polynomial::Monomial& mono);
    polynomial::Polynomial<int64_t> expandPoly(NodePtr& node);
    int factorPoly(NodePtr& node);

    std::vector<Memory>& memories() { return this->m_memory; }
    std::vector<MathFunc>& math_functions() { return this->m_functions; }
    std::vector<SpecialFunc>& special_functions() { return this->m_special_functions; }
};

}  // namespace sci_calc
#endif
