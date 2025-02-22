#pragma once
#include "../../TradingStrategies/IntraExchangeArbitration/include/Bellman_Ford.h"
#include <gtest/gtest.h>

class BellmanFordTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Граф без отрицательного цикла
        graph["A"] = {{"B", 1.0}, {"C", 4.0}};
        graph["B"] = {{"C", 2.0}, {"D", 6.0}};
        graph["C"] = {{"D", 3.0}};
        graph["D"] = {}; // Конечная вершина
    }
};
