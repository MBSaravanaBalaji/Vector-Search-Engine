#include "distance/distance.hpp"
#include <cmath>
#include <stdexcept>

namespace atlas {

// TODO: Implement your distance functions here

float dotProduct(const std::vector<float>& a, const std::vector<float>& b){
    if (a.size() != b.size()){
        throw std::invalid_argument("vectors must be same size");
    }
    
    float res = 0.0f;
    // iterates through indices for dot product
    for (size_t i = 0; i < a.size(); i++){
        res += a[i] * b[i];
    }
    return res;
}

float magnitude(const std::vector<float>& vec){
    float sumOfSquares = 0.0f;
    // iterates through indices for sum of squares
    for (float value : vec){
        sumOfSquares += value * value;
    }
    return std::sqrt(sumOfSquares);
}
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b){
    float mag_a = magnitude(a);
    float mag_b = magnitude(b);

    // edge case for zero magnitude
    if (mag_a < 1e-6 || mag_b < 1e-6){
        throw std::invalid_argument("vectors must have non-zero magnitude");
    }
    
    float dot = dotProduct(a,b);
    return dot / (mag_a * mag_b);
}
void normalize(std::vector<float>& vec){
    
    float mag = magnitude(vec);

    // check for zero mag
    if (mag < 1e-6){
        throw std::invalid_argument("vector must have non-zero magnitude");
    }

    for (float& value : vec){
        value /= mag;
    }
}



} // namespace atlas
