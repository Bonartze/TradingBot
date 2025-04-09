#pragma once
#include <string>
#include <vector>
#include "../../Common/include/Graph.h"
#include <unordered_map>
#include <limits>


class Bellman_Ford {
public:
    explicit Bellman_Ford(const Graph &graph);

    auto find_negative_cycle(const std::string &source, std::vector<std::string> &cycle) -> bool;

private:
    //! maybe rewrite with unique pointer usage
    const Graph &graph; //NOLINT
};
