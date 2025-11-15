#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct Node {
    int id;
    std::string name;
    double latitude;
    double longitude;
    
    Node(int id, const std::string& name, double lat, double lon)
        : id(id), name(name), latitude(lat), longitude(lon) {}
};

struct Edge {
    int source_id;
    int dest_id;
    double distance;
    double time;
    
    Edge(int src, int dest, double dist, double t)
        : source_id(src), dest_id(dest), distance(dist), time(t) {}
};

class Graph {
private:
    std::unordered_map<int, std::shared_ptr<Node>> nodes;
    std::unordered_map<int, std::vector<std::shared_ptr<Edge>>> adjacency_list;
    
public:
    void addNode(std::shared_ptr<Node> node);
    void addEdge(std::shared_ptr<Edge> edge);
    std::shared_ptr<Node> getNode(int id) const;
    const std::vector<std::shared_ptr<Edge>>& getEdges(int node_id) const;
    std::unordered_map<int, std::shared_ptr<Node>> getAllNodes() const;
    bool nodeExists(int id) const;
    
    void loadFromCSV(const std::string& nodes_file, const std::string& edges_file);
};

#endif