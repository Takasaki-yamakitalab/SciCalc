#include "SciCalc/sci_calc.hpp"

namespace sci_calc {
int64_t get_raw(double x) {
    double* p = &x;
    return *(int64_t*)p;
}
double raw2double(int64_t raw) {
    int64_t* p = &raw;
    return *(double*)p;
}
}
