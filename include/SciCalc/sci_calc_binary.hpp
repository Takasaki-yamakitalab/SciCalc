#pragma once

#include <ostream>
#include <istream>
#include <stdexcept>

namespace sci_calc {

template <typename T>
void writeBinary(std::ostream& os, const T& value) {
    static_assert(std::is_trivially_copyable_v<T>);
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
    if (!os) throw std::runtime_error("Binary write failed");
}

template <typename T>
void readBinary(std::istream& is, T& value) {
    static_assert(std::is_trivially_copyable_v<T>);
    is.read(reinterpret_cast<char*>(&value), sizeof(T));
    if (!is) throw std::runtime_error("Binary read failed");
}

}
