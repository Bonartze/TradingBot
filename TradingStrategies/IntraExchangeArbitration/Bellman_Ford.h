#ifndef TRADINGBOT_BELLMAN_FORD_H
#define TRADINGBOT_BELLMAN_FORD_H

#include <unordered_map>
#include <string>
#include <vector>
#include <limits>
#include "../Common/Graph.h"

class Bellman_Ford {
public:
    explicit Bellman_Ford(const Graph &graph);

    bool find_negative_cycle(const std::string &source, std::vector<std::string> &cycle);

private:
    const Graph &graph;
};

#endif