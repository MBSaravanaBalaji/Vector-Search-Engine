#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include <vector>

namespace atlas {

// computing the dot product of two vectors
float dotProduct(const std::vector<float>& a, const std::vector<float>& b);

// computing the magnitude of a vector
float magnitude(const std::vector<float>& a);

// computing the cosine similarity of two vectors
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);

// normalizing a vector 
void normalize(std::vector<float>& vec);

} // namespace atlas

#endif 
