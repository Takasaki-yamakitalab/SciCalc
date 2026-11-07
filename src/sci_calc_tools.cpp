#include <stdexcept>
#include "SciCalc/sci_calc.hpp"
#include "SciCalc/sci_calc_binary.hpp"
#include "SciCalc/sci_calc_define.hpp"
#include <iostream>
#include <string>

namespace sci_calc {
int64_t get_raw(double x) {
    double* p = &x;
    return *(int64_t*)p;
}
double raw2double(int64_t raw) {
    int64_t* p = &raw;
    return *(double*)p;
}

void saveNode(std::ostream& os, const Node& node) {
    writeBinary(os, node.type);
    writeBinary(os, static_cast<int32_t>(node.id));
    double real = node.value.real();
    double imag = node.value.imag();
    writeBinary(os, real);
    writeBinary(os, imag);
    uint64_t children_num = node.children.size();
    writeBinary(os, children_num);
    for (const auto& child : node.children) {
        if (!child) throw std::runtime_error("Null child node");
        saveNode(os, *child);
    }
}

NodePtr loadNode(std::istream& is) {
    auto node = std::make_unique<Node>();
    readBinary(is, node->type);
    int32_t id;
    readBinary(is, id);
    node->id = id;
    double real, imag;
    readBinary(is, real);
    readBinary(is, imag);
    node->value = Complex(real, imag);
    uint64_t children_num;
    readBinary(is, children_num);
    if (children_num > 1000000) throw std::runtime_error("Too many children");
    node->children.reserve(children_num);
    for (uint64_t i = 0; i < children_num; i++) {
        node->children.emplace_back(loadNode(is));
    }
    node->hash = SciCalc::node_hash(node.get());
    return node;
}

void saveOriginalFunctions(std::ostream& os, const std::unordered_map<int, OriginalFunc>& functions) {
    constexpr uint32_t MAGIC = 0x41535442;  // ASTB
    constexpr uint32_t VERSION = 1;

    writeBinary(os, MAGIC);
    writeBinary(os, VERSION);

    const size_t FUNC_SIZE = functions.size();
    writeBinary(os, FUNC_SIZE);

    for (auto& func : functions) {
        writeBinary(os, func.first);
        saveOriginalFunc(os, func.second);
    }
}

void saveOriginalFunc(std::ostream& os, const OriginalFunc& func) {
    if (!func.content) throw std::invalid_argument("Null root node");

    const size_t NAME_SIZE = func.name.size();
    writeBinary(os, NAME_SIZE);
    for (const char& c : func.name) {
        writeBinary(os, c);
    }

    const size_t VAR_ID_SIZE = func.var_id.size();
    writeBinary(os, VAR_ID_SIZE);
    for (const int& i : func.var_id) {
        writeBinary(os, i);
    }

    saveNode(os, *func.content);
}

std::unordered_map<int, OriginalFunc> loadOriginalFunctions(std::istream& is) {
    uint32_t magic, version;
    readBinary(is, magic);
    readBinary(is, version);
    if (magic != 0x41535442) throw std::runtime_error("invalid AST file");
    if (version != 1) throw std::runtime_error("Unsupported AST version");

    size_t func_size;
    readBinary(is, func_size);
    if (func_size > 1000000) throw std::runtime_error("Too many functions");

    std::unordered_map<int, OriginalFunc> ret;
    for (size_t i = 0; i < func_size; i++) {
        int id;
        readBinary(is, id);
        ret[id] = loadOriginalFunc(is);
    }
    return ret;
}

OriginalFunc loadOriginalFunc(std::istream &is) {
    size_t name_size;
    readBinary(is, name_size);
    if (name_size > 1000000) throw std::runtime_error("Too long name (" + std::to_string(name_size) + ")");
    std::string name;
    for (size_t i = 0; i < name_size; i++) {
        char c;
        readBinary(is, c);
        name += c;
    }

    size_t var_id_size;
    readBinary(is, var_id_size);
    if (var_id_size > 1000000) throw std::runtime_error("Too long var_id");
    std::vector<int> var_id;
    for (size_t i = 0; i < var_id_size; i++) {
        int id;
        readBinary(is, id);
        var_id.push_back(id);
    }

    return OriginalFunc(name, var_id, loadNode(is));
}

void saveMemory(std::ostream& os, const Memory& mem) {
    const size_t NAME_SIZE = mem.name.size();
    writeBinary(os, NAME_SIZE);
    for (const char& c : mem.name) {
        writeBinary(os, c);
    }
    writeBinary(os, mem.value.real());
    writeBinary(os, mem.value.imag());
}

void saveMemories(std::ostream& os, const std::vector<Memory>& mems) {
    constexpr uint32_t MAGIC = 0x4D454D42;  // MEMB
    constexpr uint32_t VERSION = 1;
    writeBinary(os, MAGIC);
    writeBinary(os, VERSION);

    const size_t NUM_MEMORIES = mems.size();
    if (NUM_MEMORIES > 1000000) throw std::runtime_error("Too many memories");
    writeBinary(os, NUM_MEMORIES);
    for (auto& m : mems) {
        saveMemory(os, m);
    }
}

Memory loadMemory(std::istream& is) {
    size_t name_size;
    readBinary(is, name_size);
    if (name_size > 1000000) throw std::runtime_error("Too long name (" + std::to_string(name_size) + ")");
    std::string name;
    for (size_t i = 0; i < name_size; i++) {
        char c;
        readBinary(is, c);
        name += c;
    }

    double real, imag;
    readBinary(is, real);
    readBinary(is, imag);

    return Memory(name, {real, imag});
}

std::vector<Memory> loadMemories(std::istream& is) {
    uint32_t magic, version;
    readBinary(is, magic);
    readBinary(is, version);
    if (magic != 0x4D454D42) throw std::runtime_error("invalid MEM file");
    if (version != 1) throw std::runtime_error("Unsupported MEM version");

    size_t num_memories;
    readBinary(is, num_memories);
    if (num_memories > 1000000) throw std::runtime_error("Too many memories");
    std::vector<Memory> ret;
    for (size_t i = 0; i < num_memories; i++) {
        ret.push_back(loadMemory(is));
    }
    return ret;
}

void formatMemories(std::vector<Memory>& mems) {
    for (auto it = mems.begin(); it != mems.end(); ) {
        if ('0' <= it->name[0] && it->name[0] <= '9') {
            it = mems.erase(it);
            continue;
        }
        bool del = false;
        for (const char& c : it->name) {
            if ('a' <= c && c <= 'z' ||
                'A' <= c && c <= 'Z' ||
                '0' <= c && c <= '9' ||
                c == '_' || c == '~') continue;
            del = true;
            break;
        }
        if (del) it = mems.erase(it);
        else it++;
    }
}
}
