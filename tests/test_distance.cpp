#include "distance/distance.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace atlas;

// Helper function to compare floats with tolerance
bool approxEqual(float a, float b, float epsilon = 1e-5f) {
    return std::abs(a - b) < epsilon;
}

int main() {
    std::vector<float> v1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> v2 = {4.0f, 5.0f, 6.0f};
    float result = dotProduct(v1, v2);
    assert(approxEqual(result, 32.0f));

    float result2 = magnitude(v1);
    assert(approxEqual(result2, std::sqrt(14.0f)));
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
