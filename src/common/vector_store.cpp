#include "vector_store.hpp"
#include "../distance/distance.hpp"
#include "types.hpp"
#include <algorithm>
#include <stdexcept>

namespace atlas {

VectorStore::VectorStore(size_t dimension) : dimension_(dimension) {
  if (dimension == 0) {
    throw std::invalid_argument("Dimension must be greater than 0");
  }
}

void VectorStore::addVector(VectorId id, const Vector &vec) {
  // Validate dimension
  if (vec.size() != dimension_) {
    throw std::invalid_argument("Vector dimension mismatch: expected " +
                                std::to_string(dimension_) + ", got " +
                                std::to_string(vec.size()));
  }

  // Check for duplicate ID
  if (vectors_.find(id) != vectors_.end()) {
    throw std::invalid_argument("Duplicate vector ID: " + std::to_string(id));
  }

  // Store the vector
  vectors_[id] = vec;
}

std::vector<VectorWithDistance>
VectorStore::bruteForceSearch(const Vector &query, size_t k) {
  // Validate query dimension
  if (query.size() != dimension_) {
    throw std::invalid_argument("Query dimension mismatch: expected " +
                                std::to_string(dimension_) + ", got " +
                                std::to_string(query.size()));
  }

  // Handle empty store
  if (vectors_.empty()) {
    return {};
  }

  // Compute distances to all vectors
  std::vector<VectorWithDistance> results;
  results.reserve(vectors_.size());

  for (const auto &[id, vec] : vectors_) {
    // Compute cosine similarity and convert to distance
    // Distance = 1 - similarity (so smaller distance = more similar)
    float similarity = cosineSimilarity(query, vec);
    float distance = 1.0f - similarity;
    results.emplace_back(id, distance);
  }

  // Sort by distance (ascending - closest first)
  // Use partial_sort if k is smaller than total results for efficiency
  size_t resultCount = std::min(k, results.size());

  if (resultCount < results.size()) {
    std::partial_sort(results.begin(), results.begin() + resultCount,
                      results.end());
    results.resize(resultCount);
  } else {
    std::sort(results.begin(), results.end());
  }

  return results;
}

size_t VectorStore::size() const { return vectors_.size(); }

bool VectorStore::contains(VectorId id) const {
  return vectors_.find(id) != vectors_.end();
}

const Vector &VectorStore::getVector(VectorId id) const {
  auto it = vectors_.find(id);
  if (it == vectors_.end()) {
    throw std::out_of_range("Vector ID not found: " + std::to_string(id));
  }
  return it->second;
}

size_t VectorStore::getDimension() const { return dimension_; }

} // namespace atlas
