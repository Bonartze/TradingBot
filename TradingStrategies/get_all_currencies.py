import requests

response = requests.get('https://api.binance.com/api/v3/exchangeInfo')
data = response.json()

currencies = set()
for symbol_info in data['symbols']:
    currencies.add(symbol_info['baseAsset'])
    currencies.add(symbol_info['quoteAsset'])

sorted_currencies = sorted(currencies)

with open("Common.h", "w") as file:
    file.write("#ifndef COMMON_H\n#define COMMON_H\n\n#include <unordered_set>\n\n#include <string>\n\n")
    file.write("std::unordered_set<std::string> currencies = {\n")
    for symbol in sorted_currencies:
        file.write(f"    \"{symbol}\",\n")
    file.write("    \"UNKNOWN\"\n")
    file.write("};\n\n")
    file.write("#endif\n")
