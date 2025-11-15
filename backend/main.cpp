#include "crow_all.h"
#include "graph.h"
#include "dijkstra.h"
#include "yen.h"
#include <iostream>
#include <memory>

int main() {

    // Load graph data relative to the executable
    // Executable path: pune-gps/build/backend
    // Data path:       pune-gps/data
    Graph graph;
    graph.loadFromCSV("D:/New folder/c drive/Favorites/VIT/DS/CP ALT/pune-gps/data/nodes.csv",
                  "D:/New folder/c drive/Favorites/VIT/DS/CP ALT/pune-gps/data/edges.csv");

    // App with CORS support
    crow::App<crow::CORSHandler> app;

    // Configure CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("*")           // allow all origins
        .methods("GET"_method) // allow GET
        .allow_credentials();

    // ------------------ API ROUTE ------------------
    CROW_ROUTE(app, "/api/route")
    .methods("GET"_method)
    ([&graph](const crow::request& req) {

        // Parse query params
        const char* source_name_char = req.url_params.get("source");
        const char* dest_name_char = req.url_params.get("destination");
        const char* traffic_char = req.url_params.get("traffic");

        if (!source_name_char || !dest_name_char) {
            // Bad request, missing parameters
            crow::json::wvalue err;
            err["error"] = "Missing source or destination parameters.";
            return crow::response(400, err);
        }

        std::string source_name(source_name_char);
        std::string dest_name(dest_name_char);
        bool heavy_traffic = (traffic_char && std::string(traffic_char) == "heavy");

        // Find node IDs by name
        int source_id = -1;
        int dest_id = -1;

        for (const auto& pair : graph.getAllNodes()) {
            if (pair.second->name == source_name) {
                source_id = pair.first;
            }
            if (pair.second->name == dest_name) {
                dest_id = pair.first;
            }
        }

        if (source_id == -1 || dest_id == -1) {
            crow::json::wvalue err;
            err["error"] = "Source or destination not found in graph.";
            return crow::response(404, err);
        }

        // Compute K shortest paths (1 shortest + 3 alternates)
        auto all_paths = yenKShortestPaths(graph, source_id, dest_id, 4, heavy_traffic);

        if (all_paths.empty()) {
            crow::json::wvalue err;
            err["error"] = "No routes found.";
            return crow::response(404, err);
        }

        // Construct JSON that matches frontend expectation: response.paths = [...]
        crow::json::wvalue response;
        crow::json::wvalue paths_json;

        for (size_t i = 0; i < all_paths.size(); i++) {
            const auto& p = all_paths[i];

            crow::json::wvalue route_json;
            route_json["distance"] = p.total_distance;
            route_json["time"]     = p.total_time;

            // Node IDs for frontend to look up coordinates
            crow::json::wvalue node_ids_json;
            int node_index = 0;

            for (int node_id : p.node_ids) {
                node_ids_json[node_index++] = node_id;
            }

            route_json["nodes"] = std::move(node_ids_json);

            paths_json[i] = std::move(route_json);
        }

        response["paths"] = std::move(paths_json);

        return crow::response(response);
    });

    // Run server
    app.port(18080).multithreaded().run();
    return 0;
}
