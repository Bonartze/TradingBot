#include "Bellman_Ford.h"
#include <vector>
#include <algorithm>

Bellman_Ford::Bellman_Ford(const Graph &graph) : graph(graph) {}

bool Bellman_Ford::find_negative_cycle(const std::string &source, std::vector<std::string> &cycle) {
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> predecessors;

    for (const auto &[vertex, _]: graph) {
        distances[vertex] = std::numeric_limits<double>::infinity();
        predecessors[vertex] = "";
    }
    distances[source] = 0.0;

    std::string last_updated_vertex;

    for (size_t i = 1; i <= graph.size(); ++i) {
        last_updated_vertex = "";
        for (const auto &[u, edges]: graph) {
            for (const auto &[v, weight]: edges) {
                if (distances[u] != std::numeric_limits<double>::infinity() && distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    predecessors[v] = u;
                    last_updated_vertex = v;
                }
            }
        }
    }

    if (last_updated_vertex != "") {
        for (size_t i = 0; i < graph.size(); ++i) {
            last_updated_vertex = predecessors[last_updated_vertex];
        }

        std::string start = last_updated_vertex;
        cycle.clear();
        cycle.push_back(start);
        std::string current = predecessors[start];

        while (current != start) {
            cycle.push_back(current);
            current = predecessors[current];
        }
        cycle.push_back(start);

        std::reverse(cycle.begin(), cycle.end());
        return true;
    }

    return false;
}