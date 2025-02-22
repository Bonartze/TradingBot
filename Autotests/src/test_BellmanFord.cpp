#pragma once
#include "../include/test_BellmanFord.h"

TEST_F(BellmanFordTest, NoNegativeCycle) {
    Bellman_Ford bf(graph);
    std::vector<std::string> cycle;
    EXPECT_FALSE(bf.find_negative_cycle("A", cycle));
}

TEST_F(BellmanFordTest, HasNegativeCycle) {
    graph["D"] = {{"A", -7.0}};
    Bellman_Ford bf(graph);
    std::vector<std::string> cycle;

    EXPECT_TRUE(bf.find_negative_cycle("A", cycle));
    EXPECT_FALSE(cycle.empty());
}

TEST_F(BellmanFordTest, InvalidSource) {
    Bellman_Ford bf(graph);
    std::vector<std::string> cycle;

    EXPECT_THROW(bf.find_negative_cycle("Z", cycle), std::invalid_argument);
}

TEST(BellmanFordEdgeCaseTest, EmptyGraph) {
    Graph empty_graph;
    EXPECT_THROW(Bellman_Ford bf(empty_graph), std::invalid_argument);
}
