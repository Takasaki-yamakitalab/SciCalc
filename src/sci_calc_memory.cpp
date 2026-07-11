#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
int SciCalc::set_memory() {
    auto dat = read_csv(".scicalcmem");
    for (auto& mem : dat) {
        if (mem.size() == 2) assign_memory(mem[0], std::stod(mem[1]));
        if (mem.size() == 3) assign_memory(mem[0], { std::stod(mem[1]), std::stod(mem[2]) });
    }
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
int SciCalc::save_memory() {
#ifdef __ANDROID__
    return 0;
#endif
    std::vector<std::vector<std::string>> dat;
    for (auto& mem : m_memory) {
        dat.push_back({mem.name, std::to_string(mem.value.real()), std::to_string(mem.value.imag())});
    }
    write_csv(".scicalcmem", dat);
    return 0;
}
int SciCalc::reset_memory() {
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
    save_memory();
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
}  // namespace sci_calc
