#include "SciCalc/sci_calc.hpp"
#include <fstream>
#include <sstream>

namespace sci_calc {
std::vector<std::vector<std::string>> read_csv(const std::string& fname) {
    std::vector<std::vector<std::string>> buff;
    std::ifstream file(fname);
    if (!file.is_open()) return buff;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> buff_line;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            buff_line.push_back(cell);
        }
        buff.push_back(buff_line);
    }
    return buff;
}

int write_csv(const std::string& fname,
              const std::vector<std::vector<std::string>>& dat) {
    std::ofstream file(fname);
    if (!file.is_open()) return -1;
    for (int i = 0; i < dat.size(); i++) {
        for (int j = 0; j < dat[i].size(); j++) {
            file << dat[i][j];
            if (j < dat[i].size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    return 0;
}
}  // namespace sci_calc
