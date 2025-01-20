#include "Bellman_Ford.h"
#include <vector>
#include <algorithm>
#include <exception>
#include <iostream>
Bellman_Ford::Bellman_Ford(const Graph &graph) : graph(graph) {
    if (graph.empty()) {
        throw std::invalid_argument("Graph is empty. Bellman-Ford cannot operate on an empty graph.");
    }
}

bool Bellman_Ford::find_negative_cycle(const std::string &source, std::vector<std::string> &cycle) {
    try {
        if (graph.find(source) == graph.end()) {
            throw std::invalid_argument("Source vertex '" + source + "' not found in the graph.");
        }

        std::unordered_map<std::string, double> distances;
        std::unordered_map<std::string, std::string> predecessors;

        for (const auto &[vertex, _] : graph) {
            distances[vertex] = std::numeric_limits<double>::infinity();
            predecessors[vertex] = "";
        }
        distances[source] = 0.0;

        std::string last_updated_vertex;

        for (size_t i = 1; i <= graph.size(); ++i) {
            last_updated_vertex = "";
            for (const auto &[u, edges] : graph) {
                for (const auto &[v, weight] : edges) {
                    if (distances[u] != std::numeric_limits<double>::infinity() && distances[u] + weight < distances[v]) {
                        distances[v] = distances[u] + weight;
                        predecessors[v] = u;
                        last_updated_vertex = v;
                    }
                }
            }
        }

        // Проверка на отрицательный цикл
        if (!last_updated_vertex.empty()) {
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

    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        throw;
    } catch (const std::exception &e) {
        std::cerr << "An error occurred in find_negative_cycle: " << e.what() << std::endl;
        throw;
    }
}
