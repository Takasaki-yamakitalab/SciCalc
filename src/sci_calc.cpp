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
        std::string buff(src.c_str());
        std::string dst;
        for (size_t i = 0; i < src.size(); i++) {
            if (buff[i] == '-') {
                dst += "+(-1)*";
                continue;
            }
            if (buff[i] == '/') {
                dst += "*1/";
                continue;
            }
            if (buff[i] >= '!' && buff[i] <= '~') {
                dst.push_back(buff[i]);
            }
        }
        return dst;
    }

    double SciCalc::calc(const std::string& formula) {
        m_var_id_stack.clear();
        std::string buff = rm_spaces(formula);
        NodePtr node = parse(buff);
        double ans = evaluate_node(node);
        assign_memory("preans", get_memory("ans"));
        assign_memory("ans", ans);
        return ans;
    }

    int SciCalc::set_integral_N(int N) {
        return this->m_integral_N = N;
    }
}