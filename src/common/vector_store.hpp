#pragma once

#include "../common/types.hpp"
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace atlas {

/**
 * VectorStore - Storage layer for vectors with brute-force search
 *
 * Provides efficient storage and retrieval of vectors by ID,
 * and brute-force similarity search using cosine similarity.
 */
class VectorStore {
private:
  std::unordered_map<VectorId, Vector> vectors_; // ID -> Vector mapping
  size_t dimension_;                             // Expected vector dimension

public:
  /**
   * Constructor
   * @param dimension The dimensionality of vectors to store
   */
  explicit VectorStore(size_t dimension);

  /**
   * Add a vector to the store
   * @param id Unique identifier for the vector
   * @param vec The vector data (must match store dimension)
   * @throws std::invalid_argument if dimension mismatch or duplicate ID
   */
  void addVector(VectorId id, const Vector &vec);

  /**
   * Search for the k most similar vectors using brute-force
   * @param query The query vector to search for
   * @param k Number of results to return
   * @return Vector of (id, distance) pairs, sorted by distance ascending
   */
  std::vector<VectorWithDistance> bruteForceSearch(const Vector &query,
                                                   size_t k);

  /**
   * Get the number of vectors in the store
   * @return Number of stored vectors
   */
  size_t size() const;

  /**
   * Check if a vector ID exists in the store
   * @param id The vector ID to check
   * @return true if exists, false otherwise
   */
  bool contains(VectorId id) const;

  /**
   * Retrieve a vector by ID
   * @param id The vector ID to retrieve
   * @return Reference to the vector
   * @throws std::out_of_range if ID not found
   */
  const Vector &getVector(VectorId id) const;

  /**
   * Get the dimension of vectors in this store
   * @return The vector dimension
   */
  size_t getDimension() const;
};

} // namespace atlas
