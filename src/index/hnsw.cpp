#include "hnsw.hpp"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include "../distance/distance.hpp"
#include <cmath>

namespace atlas {

HNSW::HNSW(VectorStore& store, size_t M, size_t efConstruction)
    : store_(store),
      M_(M),
      efConstruction_(efConstruction), //(default list size is 200)
      mL_(1.0 / log(M)),// normalizer
      rng_(std::random_device{}()),
      uniform_dist_(0.0, 1.0),
      entryPoint_(0),
      maxLevel_(-1) {
    // TODO: Initialize any graph data structures you design
}

void HNSW::addVector(VectorId id) {
    //select random layer for this node
    int nodeLevel = selectLevel();
    
    // create the node in our graph
    nodes_[id] = HNSWNode(nodeLevel);
    
    // first node insertion
    if (maxLevel_ == -1) {
        entryPoint_ = id;
        maxLevel_ = nodeLevel;
        return;  // First node has no neighbors to connect
    }
    
    // get the query vector for this node
    const Vector& newVec = store_.getVector(id);
    
    // find insertion point by descending from entry point
    VectorId currNode = entryPoint_;
    
    // Descend through layers above our node's level (greedy search, ef=1)
    for (int layer = maxLevel_; layer > nodeLevel; layer--) {
        auto nearest = searchLayer(newVec, {currNode}, 1, layer);
        if (!nearest.empty()) {
            currNode = nearest[0].id;
        }
    }
    
    // find neighbors and connect
    for (int layer = std::min(nodeLevel, maxLevel_); layer >= 0; layer--) {
        // Find efConstruction nearest neighbors at this layer
        auto neighbors = searchLayer(newVec, {currNode}, efConstruction_, layer);
        
        // Select top M neighbors to connect to
        size_t numConnections = std::min(M_, neighbors.size());
        
        for (size_t i = 0; i < numConnections; i++) {
            VectorId neighborId = neighbors[i].id;
            
            // Add edge
            nodes_[id].neighbors[layer].push_back(neighborId);
            nodes_[neighborId].neighbors[layer].push_back(id);
            
            // Prune neighbor's connections if they exceed M
            if (nodes_[neighborId].neighbors[layer].size() > M_) {
                // Simple pruning: keep only M closest neighbors
                auto& neighborList = nodes_[neighborId].neighbors[layer];
                const Vector& neighborVec = store_.getVector(neighborId);
                
                // calculate distances
                std::vector<std::pair<float, VectorId>> scored;
                for (auto n : neighborList) {
                    float sim = cosineSimilarity(neighborVec, store_.getVector(n));
                    scored.push_back({1.0f - sim, n});
                }
                
                // Sort by distance and keep only M closest
                std::sort(scored.begin(), scored.end());
                neighborList.clear();
                for (size_t j = 0; j < M_ && j < scored.size(); j++) {
                    neighborList.push_back(scored[j].second);
                }
            }
        }
        
        // Update entry point for next layer
        if (!neighbors.empty()) {
            currNode = neighbors[0].id;
        }
    }
    
    // update entry point if this node has higher level
    if (nodeLevel > maxLevel_) {
        entryPoint_ = id;
        maxLevel_ = nodeLevel;
    }
}

std::vector<VectorWithDistance> HNSW::search(const Vector& query, size_t k, size_t efSearch) {
    // Handle empty graph
    if (maxLevel_ == -1 || nodes_.empty()) {
        return {};
    }
    
    // start at entry point
    VectorId currNode = entryPoint_;
    
    // descend through upper layers (greedy, ef=1)
    for (int layer = maxLevel_; layer > 0; layer--) {
        auto nearest = searchLayer(query, {currNode}, 1, layer);
        if (!nearest.empty()) {
            currNode = nearest[0].id;
        }
    }
    
    // at layer 0, expanded search with efSearch candidates
    auto results = searchLayer(query, {currNode}, std::max(k, efSearch), 0);
    
    // return top k results
    if (results.size() > k) {
        results.resize(k);
    }
    
    return results;
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
    
    std::unordered_set<VectorId> visited;
    std::priority_queue<std::pair<double, VectorId>, 
                        std::vector<std::pair<double, VectorId>>, 
                        std::greater<std::pair<double, VectorId>>> candidates;  // Min-heap
    std::priority_queue<std::pair<double, VectorId>> results;  // Max-heap

    // initialize with entry points
    for(auto ep : entryPoints){
        const Vector& epVec = store_.getVector(ep);
        float similarity = cosineSimilarity(query, epVec);
        double dist = 1.0 - similarity;
        
        candidates.push({dist, ep});
        results.push({dist, ep});
        visited.insert(ep);
    }

    // Main search loop
    while(!candidates.empty()){
        auto curr = candidates.top();
        candidates.pop();

        // early termination: if closest candidate is farther than worst result, stop
        if(results.size() >= numToReturn && curr.first > results.top().first){
            break;
        }

        // explore neighbors of current node at this layer
        for(auto neighbor : nodes_[curr.second].neighbors[layer]){
            if(visited.find(neighbor) == visited.end()){
                const Vector& neighborVec = store_.getVector(neighbor);
                float similarity = cosineSimilarity(query, neighborVec);
                double dist = 1.0 - similarity;
                
                visited.insert(neighbor);
                
                // add to results if good enough
                if(results.size() < numToReturn || dist < results.top().first){
                    candidates.push({dist, neighbor});
                    results.push({dist, neighbor});
                    
                    // remove worst result if we have too many
                    if(results.size() > numToReturn){
                        results.pop();
                    }
                }
            }
        }
    }
    
    // convert heap to vector and return (closest first)
    std::vector<VectorWithDistance> result;
    while (!results.empty()) {
        auto top = results.top();
        results.pop();
        result.push_back({top.second, static_cast<float>(top.first)});
    }
    std::reverse(result.begin(), result.end());  // reverse to get closest first
    return result;
}

} // namespace atlas
