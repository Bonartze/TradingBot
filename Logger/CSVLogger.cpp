#include "CSVLogger.h"

CSVLogger::CSVLogger(const std::string &file_path) {
    csv_file.open(file_path, std::ios::out | std::ios::app);
    if (!csv_file.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + file_path);
    }
    //csv_file << "Check Type,Current Price,SMA Short,SMA Long,Entry Price,RSI\n";
    csv_file << "\nCheck Type,Current Price,SMA Short,SMA Long,RSI\n";
}

CSVLogger::~CSVLogger() {
    if (csv_file.is_open()) {
        csv_file.close();
    }
}

void CSVLogger::logRow(const std::vector<std::string> &row) {
    for (size_t i = 0; i < row.size(); ++i) {
        csv_file << row[i];
        if (i != row.size() - 1) {
            csv_file << ",";
        }
    }
    csv_file << "\n";
}
