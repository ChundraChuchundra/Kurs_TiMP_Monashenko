// MathEngine.h
#pragma once
#include <vector>
#include <cstdint>

class MathEngine {
private:
    bool checkOverflow(int64_t current, int16_t next);

public:
    int16_t calculateProduct(const std::vector<int16_t>& vector);
};