#include "SciCalc/sci_calc.hpp"
#include <algorithm>
#include <exception>
#include <fstream>

namespace sci_calc {
int SciCalc::set_memory() {
    return set_memory(m_path);
}
int SciCalc::save_memory() {
    return save_memory(m_path);
}
int SciCalc::set_memory(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (ifs.is_open()) {
        try {
            m_memory = loadMemories(ifs);
            m_original_functions = loadOriginalFunctions(ifs);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
        }
    }
    return 0;
}
int SciCalc::save_memory(const std::string& path) {

    if (is_debug) {
        std::vector<std::vector<std::string>> dat;
        for (auto& mem : m_memory) {
            dat.push_back({mem.name, std::to_string(mem.value.real()), std::to_string(mem.value.imag())});
        }
        write_csv("mems.csv", dat);
    }

    formatMemories(m_memory);

    if (is_debug) {
        std::vector<std::vector<std::string>> dat;
        for (auto& mem : m_memory) {
            dat.push_back({mem.name, std::to_string(mem.value.real()), std::to_string(mem.value.imag())});
        }
        write_csv("formated_mems.csv", dat);
    }

    std::ofstream ofs(path, std::ios::binary);
    if (ofs.is_open()) {
        try {
            saveMemories(ofs, m_memory);
            saveOriginalFunctions(ofs, m_original_functions);
            std::cerr << "saved!" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
        }
    }
    return 0;
}
int SciCalc::reset_default_mems() {
    assign_memory("ans", 0.0);
    assign_memory("preans", 0.0);
    assign_memory("~pi", std::atan(1.0) * 4.0);
    assign_memory("e", std::exp(1.0));
    assign_memory("g", 9.80665);
    assign_memory("R", 8.31446261815324);
    assign_memory("c", 299792458);
    assign_memory("G", 6.67430e-11);
    assign_memory("h", 6.62607015e-34);
    assign_memory("i", {0, 1.0});
    return 0;
}

int SciCalc::get_memory_id(const std::string& name) {
    int id = 0;
    for (id = 0; id < m_memory.size(); id++) {
        if (m_memory[id].name == name) {
            return id;
        }
    }
    // if the name is new, create a new one
    m_memory.emplace_back(name, 0.0);
    return id;
}
Complex SciCalc::get_memory(const std::string& name) {
    return m_memory[get_memory_id(name)].value;
}
Complex SciCalc::assign_memory(const std::string& name, Complex val) {
    m_memory[get_memory_id(name)].value = val;
    return val;
}

std::vector<Memory> SciCalc::get_all_memories() { return this->m_memory; }
void SciCalc::load_memories(const std::vector<Memory>& mems) {
    this->m_memory = mems;
}

int SciCalc::list_var_id(int id) {
    for (int i : this->m_var_id_stack) {
        if (i == id) return 0;
    }
    this->m_var_id_stack.push_back(id);

    auto sort_var_id = [&](int& a, int& b) -> bool {
        return m_memory[a].name < m_memory[b].name;
    };
    std::sort(m_var_id_stack.begin(), m_var_id_stack.end(), sort_var_id);

    return 1;
}
}  // namespace sci_calc
