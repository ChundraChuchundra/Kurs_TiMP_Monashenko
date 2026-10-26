#include "MathEngine.h"
#include <climits>

bool MathEngine::checkOverflow(int64_t current, int16_t next) {
    int64_t res = current * next;
    return (res > INT16_MAX || res < INT16_MIN);
}

int16_t MathEngine::calculateProduct(const std::vector<int16_t>& vector) {
    if (vector.empty()) return 0;

    int64_t result = 1;
    for (int16_t val : vector) {
        result *= val;
        // Контроль переполнения согласно ТЗ 4.2.5
        if (result > INT16_MAX) return INT16_MAX; // 2^15 - 1 (32767)
        if (result < INT16_MIN) return INT16_MIN; // -2^15 (-32768)
    }
    
    return static_cast<int16_t>(result);
}
