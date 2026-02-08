#include "index/hnsw.hpp"
#include "common/vector_store.hpp"
#include "distance/distance.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <random>

void testBasicConstruction() {
    std::cout << "Test 1: Basic Construction... ";
    
    atlas::VectorStore store(3);
    store.addVector(1, {1.0f, 0.0f, 0.0f});
    store.addVector(2, {0.0f, 1.0f, 0.0f});
    store.addVector(3, {0.0f, 0.0f, 1.0f});
    
    atlas::HNSW hnsw(store, 16, 200);
    
    std::cout << "PASSED" << std::endl;
}

void testSingleInsertion() {
    std::cout << "Test 2: Single Insertion... ";
    
    atlas::VectorStore store(3);
    store.addVector(1, {1.0f, 0.0f, 0.0f});
    
    atlas::HNSW hnsw(store, 16, 200);
    hnsw.addVector(1);
    
    std::cout << "PASSED" << std::endl;
}

void testMultipleInsertions() {
    std::cout << "Test 3: Multiple Insertions... ";
    
    atlas::VectorStore store(3);
    store.addVector(1, {1.0f, 0.0f, 0.0f});
    store.addVector(2, {0.0f, 1.0f, 0.0f});
    store.addVector(3, {0.0f, 0.0f, 1.0f});
    store.addVector(4, {1.0f, 1.0f, 0.0f});
    store.addVector(5, {0.0f, 1.0f, 1.0f});
    
    atlas::HNSW hnsw(store, 4, 50);
    hnsw.addVector(1);
    hnsw.addVector(2);
    hnsw.addVector(3);
    hnsw.addVector(4);
    hnsw.addVector(5);
    
    std::cout << "PASSED" << std::endl;
}

void testBasicSearch() {
    std::cout << "Test 4: Basic Search... ";
    
    atlas::VectorStore store(3);
    store.addVector(1, {1.0f, 0.0f, 0.0f});  // x-axis
    store.addVector(2, {0.0f, 1.0f, 0.0f});  // y-axis
    store.addVector(3, {0.0f, 0.0f, 1.0f});  // z-axis
    
    atlas::HNSW hnsw(store, 4, 50);
    hnsw.addVector(1);
    hnsw.addVector(2);
    hnsw.addVector(3);
    
    // Search for vector closest to x-axis
    atlas::Vector query = {1.0f, 0.1f, 0.0f};
    auto results = hnsw.search(query, 1, 10);
    
    assert(!results.empty() && "Search should return at least one result");
    assert(results[0].id == 1 && "Closest vector should be ID 1 (x-axis)");
    
    std::cout << "PASSED" << std::endl;
}

void testSearchRecall() {
    std::cout << "Test 5: Search Recall vs Brute Force... ";
    
    const size_t dim = 32;
    const size_t numVectors = 100;
    const size_t k = 5;
    
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    atlas::VectorStore store(dim);
    
    // Generate random vectors
    for (size_t i = 1; i <= numVectors; i++) {
        atlas::Vector vec(dim);
        for (size_t j = 0; j < dim; j++) {
            vec[j] = dist(rng);
        }
        store.addVector(i, vec);
    }
    
    // Build HNSW index
    atlas::HNSW hnsw(store, 16, 100);
    for (size_t i = 1; i <= numVectors; i++) {
        hnsw.addVector(i);
    }
    
    // Generate query
    atlas::Vector query(dim);
    for (size_t j = 0; j < dim; j++) {
        query[j] = dist(rng);
    }
    
    // Get HNSW results
    auto hnswResults = hnsw.search(query, k, 50);
    
    // Get brute force results
    auto bruteResults = store.bruteForceSearch(query, k);
    
    // Calculate recall
    size_t hits = 0;
    for (const auto& hr : hnswResults) {
        for (const auto& br : bruteResults) {
            if (hr.id == br.id) {
                hits++;
                break;
            }
        }
    }
    
    float recall = static_cast<float>(hits) / k;
    std::cout << "Recall@" << k << " = " << recall << " ";
    
    assert(recall >= 0.6f && "Recall should be at least 60%");
    
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "\n=== HNSW Index Tests ===" << std::endl;
    
    testBasicConstruction();
    testSingleInsertion();
    testMultipleInsertions();
    testBasicSearch();
    testSearchRecall();
    
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}
