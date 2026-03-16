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
        std::cout << "ans:" << std::endl;
        std::cout << "    " << calculator.calc(buff) << std::endl;
    }
    return 0;
}
