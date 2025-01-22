#pragma once
#include <fstream>
#include <string>
#include <vector>

class CSVLogger {
    std::ofstream csv_file;

public:
    explicit CSVLogger(const std::string &file_path);

    ~CSVLogger();

    void logRow(const std::vector<std::string> &row);
};
