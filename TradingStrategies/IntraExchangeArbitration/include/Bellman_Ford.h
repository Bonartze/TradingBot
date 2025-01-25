#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <limits>
#include "../../Common/include/Graph.h"

class Bellman_Ford {
public:
    explicit Bellman_Ford(const Graph &graph);

    bool find_negative_cycle(const std::string &source, std::vector<std::string> &cycle);

private:
    const Graph &graph;
};
