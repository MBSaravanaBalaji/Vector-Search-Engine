#pragma once

#include <cstdint>
#include <vector>

namespace atlas{

    //Unique identifier for vectors
    using VectorId = uint64_t;
    
    using Vector = std::vector<float>;
    
    using Distance = float;

    struct VectorWithDistance{

        VectorId id;
        Distance distance;

        // Default constructor
        VectorWithDistance() : id(0), distance(0.0f) {}

        //Constructor
        VectorWithDistance(VectorId id, Distance dist) : id(id), distance(dist){}

        //Comparison operator for sorting
        bool operator<(const VectorWithDistance& other) const {
            return distance < other.distance;
        }
    };




}
