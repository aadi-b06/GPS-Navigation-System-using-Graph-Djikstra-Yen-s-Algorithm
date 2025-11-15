#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

void Graph::addNode(std::shared_ptr<Node> node) {
    if (node) {
        nodes[node->id] = node;
    }
}

void Graph::addEdge(std::shared_ptr<Edge> edge) {
    if (edge) {
        adjacency_list[edge->source_id].push_back(edge);
    }
}

std::shared_ptr<Node> Graph::getNode(int id) const {
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Edge>>& Graph::getEdges(int node_id) const {
    static const std::vector<std::shared_ptr<Edge>> empty;
    auto it = adjacency_list.find(node_id);
    if (it != adjacency_list.end()) {
        return it->second;
    }
    return empty;
}

std::unordered_map<int, std::shared_ptr<Node>> Graph::getAllNodes() const {
    return nodes;
}

bool Graph::nodeExists(int id) const {
    return nodes.count(id) > 0;
}

void Graph::loadFromCSV(const std::string& nodes_file, const std::string& edges_file) {
    std::ifstream nodes_stream(nodes_file);
    if (!nodes_stream.is_open()) {
        std::cerr << "Error: Could not open nodes file: " << nodes_file << std::endl;
        return;
    }

    std::string line;

    // skip header of nodes.csv
    std::getline(nodes_stream, line);

    while (std::getline(nodes_stream, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id_str, name, lat_str, lon_str;

        std::getline(ss, id_str, ',');
        std::getline(ss, name, ',');
        std::getline(ss, lat_str, ',');
        std::getline(ss, lon_str, ',');

        try {
            int id = std::stoi(id_str);
            double lat = std::stod(lat_str);
            double lon = std::stod(lon_str);

            addNode(std::make_shared<Node>(id, name, lat, lon));
        }
        catch (...) {
            std::cerr << "Error parsing node line: " << line << std::endl;
        }
    }

    nodes_stream.close();

    // Load edges
    std::ifstream edges_stream(edges_file);
    if (!edges_stream.is_open()) {
        std::cerr << "Error: Could not open edges file: " << edges_file << std::endl;
        return;
    }

    // skip header
    std::getline(edges_stream, line);

    while (std::getline(edges_stream, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string src_str, dst_str, dist_str, time_str;

        std::getline(ss, src_str, ',');
        std::getline(ss, dst_str, ',');
        std::getline(ss, dist_str, ',');
        std::getline(ss, time_str, ',');

        try {
            int src = std::stoi(src_str);
            int dst = std::stoi(dst_str);
            double dist = std::stod(dist_str);
            double time = std::stod(time_str);

            // check nodes exist
            if (!nodeExists(src) || !nodeExists(dst)) {
                std::cerr << "Warning: edge refers to non-existing node: "
                          << src << " -> " << dst << std::endl;
                continue;
            }

            // add forward edge
            auto e1 = std::make_shared<Edge>(src, dst, dist, time);
            addEdge(e1);

            // add reverse edge (undirected)
            auto e2 = std::make_shared<Edge>(dst, src, dist, time);
            addEdge(e2);
        }
        catch (...) {
            std::cerr << "Error parsing edge line: " << line << std::endl;
        }
    }

    edges_stream.close();
}
