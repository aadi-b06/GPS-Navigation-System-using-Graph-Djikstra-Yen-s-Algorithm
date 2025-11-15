#include "yen.h"
#include <algorithm>
#include <set>

// Helper function to create a temporary graph with removed edges and nodes
Graph createTemporaryGraph(const Graph& original_graph, const std::set<int>& nodes_to_remove, const std::set<std::pair<int, int>>& edges_to_remove) {
    Graph temp_graph;
    for (const auto& pair : original_graph.getAllNodes()) {
        if (nodes_to_remove.find(pair.first) == nodes_to_remove.end()) {
            temp_graph.addNode(pair.second);
        }
    }

    for (const auto& pair : original_graph.getAllNodes()) {
        if (nodes_to_remove.find(pair.first) == nodes_to_remove.end()) {
            for (const auto& edge : original_graph.getEdges(pair.first)) {
                if (nodes_to_remove.find(edge->dest_id) == nodes_to_remove.end() &&
                    edges_to_remove.find({edge->source_id, edge->dest_id}) == edges_to_remove.end()) {
                    temp_graph.addEdge(edge);
                }
            }
        }
    }
    return temp_graph;
}

std::vector<PathResult> yenKShortestPaths(const Graph& graph, int start_node_id, int end_node_id, int k, bool use_traffic) {
    std::vector<PathResult> shortest_paths;
    PathResult first_path = dijkstra(graph, start_node_id, end_node_id, use_traffic);

    if (first_path.node_ids.empty()) {
        return shortest_paths; // No path found
    }

    shortest_paths.push_back(first_path);

    std::vector<PathResult> potential_paths;

    for (int i = 1; i < k; ++i) {
        if (shortest_paths.size() < i) {
            break;
        }
        PathResult previous_path = shortest_paths[i - 1];

        for (size_t j = 0; j < previous_path.node_ids.size() - 1; ++j) {
            int spur_node = previous_path.node_ids[j];
            std::vector<int> root_path(previous_path.node_ids.begin(), previous_path.node_ids.begin() + j + 1);

            std::set<std::pair<int, int>> edges_to_remove;
            for (const auto& path : shortest_paths) {
                if (path.node_ids.size() > j) {
                    bool is_same_root = true;
                    for (size_t l = 0; l <= j; ++l) {
                        if (path.node_ids[l] != root_path[l]) {
                            is_same_root = false;
                            break;
                        }
                    }
                    if (is_same_root && path.node_ids.size() > j + 1) {
                        edges_to_remove.insert({path.node_ids[j], path.node_ids[j + 1]});
                    }
                }
            }

            std::set<int> nodes_to_remove;
            for (size_t l = 0; l < j; ++l) {
                if (root_path[l] != spur_node) {
                    nodes_to_remove.insert(root_path[l]);
                }
            }

            Graph temp_graph = createTemporaryGraph(graph, nodes_to_remove, edges_to_remove);
            PathResult spur_path = dijkstra(temp_graph, spur_node, end_node_id, use_traffic);

            if (!spur_path.node_ids.empty()) {
                std::vector<int> total_path_nodes = root_path;
                total_path_nodes.insert(total_path_nodes.end(), spur_path.node_ids.begin() + 1, spur_path.node_ids.end());

                PathResult total_path;
                total_path.node_ids = total_path_nodes;
                
                // Recalculate total distance and time for the new path
                total_path.total_distance = 0.0;
                total_path.total_time = 0.0;
                for (size_t l = 0; l < total_path_nodes.size() - 1; ++l) {
                    int u = total_path_nodes[l];
                    int v = total_path_nodes[l+1];
                    for(const auto& edge : graph.getEdges(u)){
                        if(edge->dest_id == v){
                            total_path.total_distance += edge->distance;
                            total_path.total_time += use_traffic ? edge->time * 1.5 : edge->time;
                            break;
                        }
                    }
                }

                bool path_exists = false;
                for(const auto& p : potential_paths) {
                    if (p.node_ids == total_path.node_ids) {
                        path_exists = true;
                        break;
                    }
                }
                if (!path_exists) {
                    potential_paths.push_back(total_path);
                }
            }
        }

        if (potential_paths.empty()) {
            break;
        }

        std::sort(potential_paths.begin(), potential_paths.end(), [](const PathResult& a, const PathResult& b) {
            return a.total_time < b.total_time;
        });

        bool new_path_added = false;
        for(const auto& p : potential_paths) {
            bool path_exists = false;
            for(const auto& sp : shortest_paths) {
                if (sp.node_ids == p.node_ids) {
                    path_exists = true;
                    break;
                }
            }
            if (!path_exists) {
                shortest_paths.push_back(p);
                new_path_added = true;
                break;
            }
        }

        if (!new_path_added) {
            break; // No new shortest paths found
        }
    }

    return shortest_paths;
}