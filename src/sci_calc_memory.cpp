#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
	int SciCalc::set_memory() {
		auto dat = read_csv(".scicalcmem");
		for (auto& mem : dat) {
			if (mem.size() != 2) continue;
			assign_memory(mem[0], std::stod(mem[1]));
		}
		assign_memory("ans", 0.0);
		assign_memory("preans", 0.0);
		assign_memory("pi", 3.14159265358979323846264338327950288);
		assign_memory("e", 2.718281828459045235360287471532);
		assign_memory("g", 9.80665);
		assign_memory("R", 8.31446261815324);
		assign_memory("c_0", 299792458);
		assign_memory("mu_0", 1.25663706127e-6);
		assign_memory("epsilon_0", 8.8541878188e-12);
		assign_memory("Z_0", 376.730313412);
		assign_memory("G", 6.67430e-11);
		assign_memory("h", 6.62607015e-34);
		assign_memory("hbar", 1.054571817e-34);
		return 0;
	}
	int SciCalc::save_memory() {
		std::vector<std::vector<std::string>> dat;
		for (auto& mem : m_memory) {
			dat.push_back({ mem.name, std::to_string(mem.value) });
		}
		write_csv(".scicalcmem", dat);
		return 0;
	}
	int SciCalc::reset_memory() {
		m_memory.clear();
		m_memory.emplace_back("ans", 0.0);
		m_memory.emplace_back("preans", 0.0);
		m_memory.emplace_back("pi", 3.14159265358979323846264338327950288);
		m_memory.emplace_back("e", 2.718281828459045235360287471532);
		m_memory.emplace_back("g", 9.80665);
		m_memory.emplace_back("R", 8.31446261815324);
		m_memory.emplace_back("c_0", 299792458);
		m_memory.emplace_back("mu_0", 1.25663706127e-6);
		m_memory.emplace_back("epsilon_0", 8.8541878188e-12);
		m_memory.emplace_back("Z_0", 376.730313412);
		m_memory.emplace_back("G", 6.67430e-11);
		m_memory.emplace_back("h", 6.62607015e-34);
		m_memory.emplace_back("hbar", 1.054571817e-34);
		save_memory();
		return 0;
	}

	size_t SciCalc::get_memory_id(const std::string& name) {
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
	double SciCalc::get_memory(const std::string& name) {
		return m_memory[get_memory_id(name)].value;
	}
	double SciCalc::assign_memory(const std::string& name, double val) {
		m_memory[get_memory_id(name)].value = val;
		return val;
	}
}