#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include <vector>
#include <string>

struct PathResult {
    std::vector<int> node_ids;
    double total_distance;
    double total_time;
};

PathResult dijkstra(const Graph& graph, int start_node_id, int end_node_id, bool use_traffic);

#endif