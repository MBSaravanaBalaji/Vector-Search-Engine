#pragma once

#include "../common/types.hpp"
#include "../common/vector_store.hpp"
#include <vector>
#include <unordered_map>
#include <random>
#include <cmath>

namespace atlas {

/**
 * HNSW (Hierarchical Navigable Small World) Index
 * 
 * TODO: Design questions to answer:
 * 1. How should we represent a node with multiple layers?
 * 2. How do we store neighbors for each layer?
 * 3. What's the data structure for the entire graph?
 */

class HNSW {
public:
    /**
     * Constructor
     * 
     * @param store Reference to VectorStore (where actual vectors live)
     * @param M Maximum number of connections per node (typical: 16-48)
     * @param efConstruction Size of dynamic candidate list during construction (typical: 100-400)
     */
    HNSW(VectorStore& store, size_t M = 16, size_t efConstruction = 200);

    /**
     * Add a vector to the HNSW index
     * 
     * @param id VectorId that exists in the VectorStore
     * 
     * TODO Steps to implement:
     * 1. Select layer for this node using exponential decay
     * 2. Find insertion point by searching from top layer down
     * 3. Insert node and connect to neighbors at each layer
     */
    void addVector(VectorId id);

    /**
     * Search for k nearest neighbors
     * 
     * @param query Query vector
     * @param k Number of nearest neighbors to return
     * @param efSearch Size of dynamic candidate list (higher = better recall, slower)
     * @return Vector of k nearest neighbors with distances
     * 
     * TODO Steps to implement:
     * 1. Start at entry point (top layer)
     * 2. Greedily descend through layers
     * 3. At layer 0, expand search to ef candidates
     * 4. Return top k results
     */
    std::vector<VectorWithDistance> search(const Vector& query, size_t k, size_t efSearch);

private:
    // Reference to the vector storage
    VectorStore& store_;
    
    // HNSW parameters
    size_t M_;              // Max connections per layer
    size_t efConstruction_; // Size of dynamic list during construction
    double mL_;             // Normalization factor for level generation: 1/ln(M)
    
    // Random number generation for layer selection
    std::mt19937 rng_;
    std::uniform_real_distribution<double> uniform_dist_;
    
    // TODO: Design the node structure
    struct HNSWNode{
        int topLayer;

        std::vector<std::vector<VectorId>> neighbors;

        // Default constructor
        HNSWNode() : topLayer(0), neighbors(1) {}
        
        // Constructor with layer
        HNSWNode(int layer) : topLayer(layer), neighbors(layer + 1){}
    };
    
    std::unordered_map<VectorId, HNSWNode> nodes_;
    VectorId entryPoint_;   // Entry point for search (node with highest layer)
    int maxLevel_;          // Current maximum layer in the graph
    
    /**
     * Randomly select the top layer for a new node
     * Uses exponential decay: P(level = l) ~ (1/M)^l
     */
    int selectLevel();
    
    /**
     * Search for nearest neighbors within a single layer
     * This is the core algorithm that gets reused during insertion and search
     * 
     * TODO: This is the MOST IMPORTANT function to understand!
     * 
     * @param query Query vector
     * @param entryPoints Starting points for search in this layer
     * @param numToReturn How many closest neighbors to return
     * @param layer Which layer to search in
     * @return Closest neighbors found in this layer
     */
    std::vector<VectorWithDistance> searchLayer(
        const Vector& query,
        const std::vector<VectorId>& entryPoints,
        size_t numToReturn,
        int layer
    );
};

} // namespace atlas
