#include <iostream>
#include <string>
#include "../include/SciCalc/sci_calc.hpp"

int main(void) {
    sci_calc::SciCalc calculator;
    std::string buff;
    while (1) {
        std::cout << "input formula (or type 'exit' or 'end' to exit).:" << std::endl;
        std::cout << ">> ";
        std::cin >> buff;
        if (buff == "end" || buff == "exit") break;
        sci_calc::Complex ans = calculator.calc(buff);
        std::cout << "ans:" << std::endl;
        std::cout << "    " << (ans == 0.0 ? "0" : calculator.to_string_g(ans)) << std::endl;
    }
    return 0;
}
