#include <iostream>
#include <strategies_api.h>
#include "../Logger/include/CSVLogger.h"

int main() {
    // Создание экземпляра ScalpingStr через правильную функцию
    ScalpingStr* scalping = CreateScalpingStr();
    if (!scalping) {
        std::cerr << "Failed to create ScalpingStr instance." << std::endl;
        return 1;
    }
    std::cout << "ScalpingStr instance created." << std::endl;

    // Фиктивный массив цен
    double prices[] = {100.0, 101.5, 102.0, 103.0, 102.5, 104.0};
    size_t count = sizeof(prices) / sizeof(prices[0]);


    // Вызов функции execute с массивом цен
    double scalping_profit = ScalpingStr_WrapperExecute(scalping,20, prices, count, nullptr);
    std::cout << "Scalping strategy executed. Profit: " << scalping_profit << std::endl;

    // Уничтожение экземпляра ScalpingStr
    DestroyScalpingStr(scalping);
    std::cout << "ScalpingStr instance destroyed." << std::endl;

    return 0;
}
