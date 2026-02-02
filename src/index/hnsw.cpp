#include "hnsw.hpp"

namespace atlas {

HNSW::HNSW(VectorStore& store, size_t M, size_t efConstruction)
    : store_(store),
      M_(M),
      efConstruction_(efConstruction),
      mL_(1.0 / log(M)),
      rng_(std::random_device{}()),
      uniform_dist_(0.0, 1.0),
      entryPoint_(0),
      maxLevel_(-1) {
    // TODO: Initialize any graph data structures you design
}

void HNSW::addVector(VectorId id) {
    // TODO: Implement insertion
    // Step 1: Select layer for this node
    // Step 2: Find insertion point
    // Step 3: Connect to neighbors
}

std::vector<VectorWithDistance> HNSW::search(const Vector& query, size_t k, size_t efSearch) {
    // TODO: Implement multi-layer search
    // Step 1: Start at entry point
    // Step 2: Descend through layers
    // Step 3: Return top k results
    return {};
}

int HNSW::selectLevel() {
    double r = uniform_dist_(rng_);
    return static_cast<int>(-log(r) * mL_);
}

std::vector<VectorWithDistance> HNSW::searchLayer(
    const Vector& query,
    const std::vector<VectorId>& entryPoints,
    size_t numToReturn,
    int layer) {
    // TODO: Implement single-layer search
    // This is the CORE algorithm!
    return {};
}

} // namespace atlas
