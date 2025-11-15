#ifndef YEN_H
#define YEN_H

#include "graph.h"
#include "dijkstra.h"
#include <vector>

std::vector<PathResult> yenKShortestPaths(const Graph& graph, int start_node_id, int end_node_id, int k, bool use_traffic);

#endif