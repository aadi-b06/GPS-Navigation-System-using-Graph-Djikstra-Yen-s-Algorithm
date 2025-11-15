#include "dijkstra.h"
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>

PathResult dijkstra(const Graph& graph, int start_node_id, int end_node_id, bool use_traffic) {
    std::unordered_map<int, double> distances;
    std::unordered_map<int, int> predecessors;
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> pq;

    for (const auto& pair : graph.getAllNodes()) {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }

    if (!graph.nodeExists(start_node_id) || !graph.nodeExists(end_node_id)) {
        return {{}, -1.0, -1.0}; // Return empty path if nodes don't exist
    }

    distances[start_node_id] = 0.0;
    pq.push({0.0, start_node_id});

    while (!pq.empty()) {
        double current_dist = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (u == end_node_id) {
            break; // Found the shortest path
        }

        if (current_dist > distances[u]) {
            continue;
        }

        for (const auto& edge : graph.getEdges(u)) {
            int v = edge->dest_id;
            double weight = use_traffic ? edge->time * 1.5 : edge->time;

            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                pq.push({distances[v], v});
            }
        }
    }

    PathResult result;
    if (distances[end_node_id] == std::numeric_limits<double>::infinity()) {
        return {{}, -1.0, -1.0}; // No path found
    }

    std::vector<int> path;
    int current_node = end_node_id;
    while (current_node != start_node_id) {
        path.push_back(current_node);
        if (predecessors.find(current_node) == predecessors.end()) {
            return {{}, -1.0, -1.0}; // Path is broken
        }
        current_node = predecessors[current_node];
    }
    path.push_back(start_node_id);
    std::reverse(path.begin(), path.end());

    result.node_ids = path;
    result.total_time = distances[end_node_id];

    // Calculate total distance
    result.total_distance = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i];
        int v = path[i + 1];
        for (const auto& edge : graph.getEdges(u)) {
            if (edge->dest_id == v) {
                result.total_distance += edge->distance;
                break;
            }
        }
    }

    return result;
}