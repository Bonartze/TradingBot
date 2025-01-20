#include "ScalpingStr.h"

std::vector<Candle> ScalpingStr::loadCandles(const std::string& filename) {
    std::vector<Candle> candles;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string value;
      Candle candle;

      std::getline(ss, value, ','); candle.timestamp = std::stol(value);
      std::getline(ss, value, ','); candle.open = std::stod(value);
      std::getline(ss, value, ','); candle.high = std::stod(value);
      std::getline(ss, value, ','); candle.low = std::stod(value);
      std::getline(ss, value, ','); candle.close = std::stod(value);
      std::getline(ss, value, ','); candle.volume = std::stod(value);

      candles.push_back(candle);
    }
    return candles;
  }