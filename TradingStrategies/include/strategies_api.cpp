
#include "strategies_api.h"

#include "../IntraExchangeArbitration/include/Arbitrage.h"
#include "../InterExchangeArbitration/include/ExchangeAPI.h"
#include "../Scalping/include/ScalpingStr.h"
#include "../MeanReversionStrategy/include/MeanReverseStr.h"
#include "../BayesianSignalFiltering/include/BayesianSignalFiltering.h"
#include "../ArimaGarchAdaptiveTrading/include/ArimaGarchAdaptive.h"
#include "../../Logger/include/CSVLogger.h"

#include <unordered_set>
#include <string>
#include <exception>

#ifdef __cplusplus
extern "C" {
#endif

STRATEGIES_API Arbitrage* CreateArbitrage(const char** symbols, size_t symbol_count,
                                          int version, const char* api_key, const char* secret_key) {
    try {
        std::unordered_set<std::string> symSet;
        for (size_t i = 0; i < symbol_count; ++i) {
            symSet.insert(symbols[i]);
        }
        return new Arbitrage(symSet, version, api_key, secret_key);
    } catch (const std::exception &e) {
        return nullptr;
    }
}

STRATEGIES_API void DestroyArbitrage(Arbitrage* instance) {
    delete instance;
}

STRATEGIES_API void Arbitrage_FindOpportunities(Arbitrage* instance, const char* source, double amount) {
    if (instance && source) {
        instance->find_arbitrage_opportunities(source, amount);
    }
}


// ScalpingStr API


STRATEGIES_API ScalpingStr* CreateScalpingStr() {
    return new ScalpingStr();
}

STRATEGIES_API ScalpingStr* CreateScalpingStrEx(const TradingParams* params, double balance,
                                               bool position_open, double entry_price,
                                               double asset_quantity, const char* key,
                                               const char* secret, const char* symbol) {
    if (params && key && secret && symbol) {
        return new ScalpingStr(*params, balance, position_open, entry_price, asset_quantity, key, secret, symbol);
    }
    return nullptr;
}

STRATEGIES_API void DestroyScalpingStr(ScalpingStr* instance) {
    delete instance;
}

STRATEGIES_API double ScalpingStr_WrapperExecute(ScalpingStr* instance, size_t window_size,
                                                  const double* prices, size_t count,
                                                  CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        auto result = instance->wrapper_execute(window_size, vec, *logger);
        return result.first;
    }
    return 0.0;
}


// MeanReverseStrategy API


STRATEGIES_API MeanReverseStrategy* CreateMeanReverseStrategy() {
    return new MeanReverseStrategy();
}

STRATEGIES_API MeanReverseStrategy* CreateMeanReverseStrategyEx(const TradingParams* params, double balance,
                                                               bool position_open, double entry_price,
                                                               double asset_quantity, const char* key, const char* secret, const char* symbol) {
    if (params) {
        return new MeanReverseStrategy(*params, balance, position_open, entry_price, asset_quantity, key, secret, symbol);
    }
    return nullptr;
}

STRATEGIES_API void DestroyMeanReverseStrategy(MeanReverseStrategy* instance) {
    delete instance;
}

STRATEGIES_API double MeanReverseStrategy_Execute(MeanReverseStrategy* instance,
                                                  const double* prices, size_t count,
                                                  CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        return instance->execute(vec, *logger);
    }
    return 0.0;
}

STRATEGIES_API double MeanReverseStrategy_WrapperExecute(MeanReverseStrategy* instance, size_t window_size,
                                                         const double* prices, size_t count,
                                                         CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        auto result = instance->wrapper_execute(window_size, vec, *logger);
        return result.first;
    }
    return 0.0;
}


// BayesianSignalFiltering API


STRATEGIES_API BayesianSignalFiltering* CreateBayesianStrategy() {
    return new BayesianSignalFiltering();
}

STRATEGIES_API BayesianSignalFiltering* CreateBayesianStrategyEx(const TradingParams* params, double balance,
                                                                bool position_open, double quantity,
                                                                double entry_price, const char* key,
                                                                const char* secret, const char* symbol) {
    if (params && key && secret && symbol) {
        return new BayesianSignalFiltering(*params, balance, position_open, quantity, entry_price, key, secret, symbol);
    }
    return nullptr;
}

STRATEGIES_API void DestroyBayesianStrategy(BayesianSignalFiltering* instance) {
    delete instance;
}

STRATEGIES_API double BayesianStrategy_Execute(BayesianSignalFiltering* instance,
                                               const double* prices, size_t count,
                                               CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        return instance->execute(vec, *logger);
    }
    return 0.0;
}

STRATEGIES_API double BayesianStrategy_WrapperExecute(BayesianSignalFiltering* instance, size_t window_size,
                                                      const double* prices, size_t count,
                                                      CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        auto result = instance->wrapper_execute(window_size, vec, *logger);
        return result.first;
    }
    return 0.0;
}


// ArimaGarchAdaptive API


STRATEGIES_API ArimaGarchAdaptive* CreateArimaGarchAdaptive(const char* filepath) {
    if (filepath) {
        return new ArimaGarchAdaptive(filepath);
    }
    return nullptr;
}

STRATEGIES_API ArimaGarchAdaptive* CreateArimaGarchAdaptiveEx(const char* filepath, const TradingParams* params,
                                                              double balance, bool position_open,
                                                              double entry_price, double asset_quantity,
                                                              const char* key, const char* secret,
                                                              const char* symbol) {
    if (filepath && params && key && secret && symbol) {
        return new ArimaGarchAdaptive(filepath, *params, balance, position_open, entry_price, asset_quantity, key, secret, symbol);
    }
    return nullptr;
}

STRATEGIES_API void DestroyArimaGarchAdaptive(ArimaGarchAdaptive* instance) {
    delete instance;
}

STRATEGIES_API double ArimaGarchStrategy_Execute(ArimaGarchAdaptive* instance,
                                                 const double* prices, size_t count,
                                                 CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        return instance->execute(vec, *logger);
    }
    return 0.0;
}

STRATEGIES_API double ArimaGarchStrategy_WrapperExecute(ArimaGarchAdaptive* instance, size_t window_size,
                                                        const double* prices, size_t count,
                                                        CSVLogger* logger) {
    if (instance && prices && logger) {
        std::vector<double> vec(prices, prices + count);
        auto result = instance->wrapper_execute(window_size, vec, *logger);
        return result.first;
    }
    return 0.0;
}

// ExchangeAPI API

STRATEGIES_API void RunInterexchangeArbitrage(const char* crypto_pair) {
    if (crypto_pair) {
        ExchangeAPI::run_interexchange_arbitrage(crypto_pair);
    }
}

#ifdef __cplusplus
}
#endif

