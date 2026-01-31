#include "distance/distance.hpp"
#include "common/vector_store.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include "common/types.hpp"

using namespace atlas;

// Helper function to compare floats with tolerance
bool approxEqual(float a, float b, float epsilon = 1e-5f) {
  return std::abs(a - b) < epsilon;
}

void testEmptyStore() {
  std::cout << "Testing empty store search... ";

  VectorStore store(3);
  Vector query = {1.0f, 0.0f, 0.0f};
  auto results = store.bruteForceSearch(query, 5);

  assert(results.empty());
  assert(store.size() == 0);

  std::cout << "PASSED" << std::endl;
}

void testAddSingleVector() {
  std::cout << "Testing add single vector... ";

  VectorStore store(3);
  Vector vec = {1.0f, 2.0f, 3.0f};

  store.addVector(1, vec);

  assert(store.size() == 1);
  assert(store.contains(1));
  assert(!store.contains(2));

  const Vector &retrieved = store.getVector(1);
  assert(retrieved.size() == 3);
  assert(approxEqual(retrieved[0], 1.0f));
  assert(approxEqual(retrieved[1], 2.0f));
  assert(approxEqual(retrieved[2], 3.0f));

  std::cout << "PASSED" << std::endl;
}

void testAddMultipleVectors() {
  std::cout << "Testing add multiple vectors... ";

  VectorStore store(3);

  store.addVector(1, {1.0f, 0.0f, 0.0f});
  store.addVector(2, {0.0f, 1.0f, 0.0f});
  store.addVector(3, {0.0f, 0.0f, 1.0f});

  assert(store.size() == 3);
  assert(store.contains(1));
  assert(store.contains(2));
  assert(store.contains(3));

  std::cout << "PASSED" << std::endl;
}

void testBruteForceSearch() {
  std::cout << "Testing brute force search... ";

  VectorStore store(3);

  // Add unit vectors along each axis
  store.addVector(1, {1.0f, 0.0f, 0.0f});     // x-axis
  store.addVector(2, {0.0f, 1.0f, 0.0f});     // y-axis
  store.addVector(3, {0.0f, 0.0f, 1.0f});     // z-axis
  store.addVector(4, {0.707f, 0.707f, 0.0f}); // 45 degrees between x and y

  // Search for vector closest to x-axis
  Vector query = {1.0f, 0.0f, 0.0f};
  auto results = store.bruteForceSearch(query, 2);

  assert(results.size() == 2);
  // First result should be the x-axis vector (ID 1) with distance ~0
  assert(results[0].id == 1);
  assert(approxEqual(results[0].distance, 0.0f, 0.001f));
  // Second should be the 45-degree vector (ID 4)
  assert(results[1].id == 4);

  std::cout << "PASSED" << std::endl;
}

void testSearchReturnsCorrectTopK() {
  std::cout << "Testing search returns correct top-k... ";

  VectorStore store(3);

  // Add 5 vectors
  store.addVector(1, {1.0f, 0.0f, 0.0f});
  store.addVector(2, {0.9f, 0.1f, 0.0f});
  store.addVector(3, {0.8f, 0.2f, 0.0f});
  store.addVector(4, {0.0f, 1.0f, 0.0f});
  store.addVector(5, {0.0f, 0.0f, 1.0f});

  Vector query = {1.0f, 0.0f, 0.0f};

  // Ask for top 3
  auto results = store.bruteForceSearch(query, 3);
  assert(results.size() == 3);

  // Ask for more than available
  results = store.bruteForceSearch(query, 10);
  assert(results.size() == 5);

  // Ask for top 1
  results = store.bruteForceSearch(query, 1);
  assert(results.size() == 1);
  assert(results[0].id == 1); // Exact match should be first

  std::cout << "PASSED" << std::endl;
}

void testDimensionValidation() {
  std::cout << "Testing dimension validation... ";

  VectorStore store(3);

  // Try to add vector with wrong dimension
  bool exceptionThrown = false;
  try {
    store.addVector(1, {1.0f, 2.0f}); // Only 2 dimensions
  } catch (const std::invalid_argument &e) {
    exceptionThrown = true;
  }
  assert(exceptionThrown);

  // Try to search with wrong dimension
  exceptionThrown = false;
  try {
    store.bruteForceSearch({1.0f, 2.0f}, 5);
  } catch (const std::invalid_argument &e) {
    exceptionThrown = true;
  }
  assert(exceptionThrown);

  std::cout << "PASSED" << std::endl;
}

void testDuplicateIdHandling() {
  std::cout << "Testing duplicate ID handling... ";

  VectorStore store(3);
  store.addVector(1, {1.0f, 2.0f, 3.0f});

  // Try to add another vector with same ID
  bool exceptionThrown = false;
  try {
    store.addVector(1, {4.0f, 5.0f, 6.0f});
  } catch (const std::invalid_argument &e) {
    exceptionThrown = true;
  }
  assert(exceptionThrown);

  // Original vector should still be there
  const Vector &vec = store.getVector(1);
  assert(approxEqual(vec[0], 1.0f));

  std::cout << "PASSED" << std::endl;
}

void testGetNonexistentVector() {
  std::cout << "Testing get nonexistent vector... ";

  VectorStore store(3);

  bool exceptionThrown = false;
  try {
    store.getVector(999);
  } catch (const std::out_of_range &e) {
    exceptionThrown = true;
  }
  assert(exceptionThrown);

  std::cout << "PASSED" << std::endl;
}

int main() {
  std::cout << "========================================" << std::endl;
  std::cout << "  VectorStore Unit Tests" << std::endl;
  std::cout << "========================================" << std::endl;

  testEmptyStore();
  testAddSingleVector();
  testAddMultipleVectors();
  testBruteForceSearch();
  testSearchReturnsCorrectTopK();
  testDimensionValidation();
  testDuplicateIdHandling();
  testGetNonexistentVector();

  std::cout << "========================================" << std::endl;
  std::cout << "All tests passed!" << std::endl;
  std::cout << "========================================" << std::endl;

  return 0;
}
