#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Common/Common.h"

class ScalpingStr {
  private:

public:
  std::vector<Candle> loadCandles(const std::string& filename);

};



