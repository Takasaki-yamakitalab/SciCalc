#include <cmath>
#include <stdio.h>
#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
	int SciCalc::set_math_func_list() {
		// mathematical functions
		// sort by longest name first
        // all mathematical function names begin with '@'.
		this->m_functions.emplace_back("@arcsinh", [](double x) { return asinh(x); });
		this->m_functions.emplace_back("@arccosh", [](double x) { return acosh(x); });
		this->m_functions.emplace_back("@arctanh", [](double x) { return atanh(x); });
		this->m_functions.emplace_back("@arcsin", [](double x) { return asin(x); } );
		this->m_functions.emplace_back("@arccos", [](double x) { return acos(x); });
		this->m_functions.emplace_back("@arctan", [](double x) { return atan(x); } );
		this->m_functions.emplace_back("@gamma", [](double x) { return tgamma(x); } );
		this->m_functions.emplace_back("@sinh", [](double x) { return sinh(x); } );
		this->m_functions.emplace_back("@cosh", [](double x) { return cosh(x); } );
		this->m_functions.emplace_back("@tanh", [](double x) { return tanh(x); } );
		this->m_functions.emplace_back("@sqrt", [](double x) { return sqrt(x); } );
		this->m_functions.emplace_back("@cbrt", [](double x) { return cbrt(x); } );
		this->m_functions.emplace_back("@sin", [](double x) { return sin(x); } );
		this->m_functions.emplace_back("@cos", [](double x) { return cos(x); } );
		this->m_functions.emplace_back("@tan", [](double x) { return tan(x); } );
		this->m_functions.emplace_back("@log", [](double x) { return log10(x); } );
		this->m_functions.emplace_back("@exp", [](double x) { return exp(x); } );
		this->m_functions.emplace_back("@ln", [](double x) { return log(x); } );

		// error
		this->m_functions.emplace_back(
			"",
			[](double x) {
                (void)x;
				fprintf(stderr, "[ERROR] Undefined function name.\n");
				return NAN;
			});

		return 0;
	}
	int SciCalc::add_math_func_to_list(const MathFunc& func) {
		for (int i = 0; i < m_functions.size(); i++) {
			if (func.name.size() <= m_functions[i].name.size()) {
				m_functions.insert(m_functions.begin() + i, func);
				return 0;
			}
		}
		m_functions.push_back(func);
		return 0;
	}

    bool SciCalc::ends_with_math_func(const std::string& str) {
        if (str.find("@") == std::string::npos) return false;
        for (int i = 0; i < m_functions.size(); i++) {
            if (str.rfind(m_functions[i].name) == str.size() - m_functions[i].name.size()) {
                return true;
            }
        }
        return false;
    }

	int SciCalc::detect_func(const std::string& func) {
		int id;
		for (id = 0; id < m_functions.size(); id++) {
			if (func.find(m_functions[id].name) == 0) return id;
		}
		return m_functions.size() - 1;
	}

	std::string SciCalc::rm_func_name(std::string& func) {
		int id = detect_func(func);
		int nameLen = m_functions[id].name.size();
		if (nameLen == 0) return func.erase(0);
		return func.erase(0, nameLen);
	}

	int SciCalc::permutation(double n, double r){
		return permutation((int)(n + 0.5), (int)(r + 0.5));
	}
	int SciCalc::combination(double n, double r){
		return combination((int)(n + 0.5), (int)(r + 0.5));
	}

	int SciCalc::permutation(int n, int r) {
		int ans = 1;
		for (int i = 0; i < r; i++) {
			ans *= (n - i);
		}
		return ans;
	}
	int SciCalc::combination(int n, int r) {
		return permutation(n, r) / permutation(r, r);
	}
}
