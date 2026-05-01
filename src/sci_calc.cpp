#include <cmath>
#include <stdio.h>
#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
    SciCalc::SciCalc() : is_debug(false) {
        init();
    }
    SciCalc::SciCalc(int N) : is_debug(false), m_integral_N(N) {
        init();
    }
    SciCalc::SciCalc(bool debug) : is_debug(debug) {
        init();
    }
    SciCalc::SciCalc(int N, bool debug) : is_debug(debug), m_integral_N(N) {
        init();
    }
    int SciCalc::init() {
        this->m_var_id_stack.clear();
        this->set_memory();
        this->set_math_func_list();
        this->set_special_func_list();
        return 0;
    }
    SciCalc::~SciCalc() {
        save_memory();
    }

    std::string SciCalc::rm_spaces(const std::string& src) {
        std::string buff;
        std::string dst;
        for (size_t i = 0; i < src.size(); i++) {
            if (src[i] >= '!' && src[i] <= '~') {
                buff.push_back(src[i]);
            }
        }
        for (size_t i = 0; i < buff.size(); i++) {
            if (buff[i] == '(' ||
                buff[i] == '@' ||
                buff[i] == '\\') {
                if (dst.size() > 0 &&
                    dst[dst.size() - 1] != '(' &&
                    dst[dst.size() - 1] != '{' &&
                    dst[dst.size() - 1] != '[' &&
                    dst[dst.size() - 1] != '+' &&
                    dst[dst.size() - 1] != '-' &&
                    dst[dst.size() - 1] != '*' &&
                    dst[dst.size() - 1] != '/' &&
                    dst[dst.size() - 1] != '^' &&
                    !ends_with_math_func(dst)) {
                        dst += "*";
                    }
                dst.push_back(buff[i]);
                continue;
            }
            if (buff[i] == ')') {
                dst.push_back(buff[i]);
                if (i < src.size() - 1 &&
                    buff[i + 1] != ')' &&
                    buff[i + 1] != '}' &&
                    buff[i + 1] != ']' &&
                    buff[i + 1] != '+' &&
                    buff[i + 1] != '-' &&
                    buff[i + 1] != '*' &&
                    buff[i + 1] != '/' &&
                    buff[i + 1] != '^') {
                        dst += "*";
                    }
                continue;
            }
            dst.push_back(buff[i]);
        }
        return dst;
    }

    double SciCalc::calc(const std::string& formula) {
        m_var_id_stack.clear();
        if (formula != this->formula_cash) {
            this->formula_cash = formula;
            std::string buff = rm_spaces(formula);
            this->ast_cash = parse(buff);
        }
        double ans = evaluate_node(this->ast_cash);
        assign_memory("preans", get_memory("ans"));
        assign_memory("ans", ans);
        return ans;
    }

    std::string SciCalc::expand(const std::string& formula) {
        m_var_id_stack.clear();
        if (formula != this->formula_cash) {
            this->formula_cash = formula;
            std::string buff = rm_spaces(formula);
            this->ast_cash = parse(buff);
        }
        NodePtr ans = std::make_unique<Node>(*this->ast_cash.get());
        expand(ans);
        return to_string(ans);
    }
}