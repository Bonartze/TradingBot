#pragma once

#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#ifdef STRATEGIES_API_EXPORT
#define STRATEGIES_API __declspec(dllexport)
#else
#define STRATEGIES_API __declspec(dllimport)
#endif
#elif defined(__linux__) || defined(__unix__)
#define STRATEGIES_API __attribute__((visibility("default")))
#else
#define STRATEGIES_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>


typedef struct Arbitrage Arbitrage;
typedef struct ScalpingStr ScalpingStr;
typedef struct MeanReverseStrategy MeanReverseStrategy;
typedef struct BayesianSignalFiltering BayesianSignalFiltering;
typedef struct ArimaGarchAdaptive ArimaGarchAdaptive;
typedef struct CSVLogger CSVLogger;
typedef struct TradingParams TradingParams;
typedef struct ExchangeAPI ExchangeAPI;


STRATEGIES_API Arbitrage *CreateArbitrage(const char **symbols, size_t symbol_count,
                                          int version, const char *api_key, const char *secret_key);
STRATEGIES_API void DestroyArbitrage(Arbitrage *instance);

STRATEGIES_API void Arbitrage_FindOpportunities(Arbitrage *instance, const char *source, double amount);


STRATEGIES_API ScalpingStr *CreateScalpingStr();

STRATEGIES_API ScalpingStr *CreateScalpingStrEx(const TradingParams *params, double balance,
                                                bool position_open, double entry_price,
                                                double asset_quantity, const char *key,
                                                const char *secret, const char *symbol, bool is_testing = false);
STRATEGIES_API void DestroyScalpingStr(ScalpingStr *instance);


STRATEGIES_API double ScalpingStr_WrapperExecute(ScalpingStr *instance, size_t window_size,
                                                 const double *prices, size_t count,
                                                 CSVLogger &logger);

STRATEGIES_API MeanReverseStrategy *CreateMeanReverseStrategy();

STRATEGIES_API MeanReverseStrategy *CreateMeanReverseStrategyEx(const TradingParams *params, double balance,
                                                                bool position_open, double entry_price,
                                                                double asset_quantity, const char *key,
                                                                const char *secret, const char *symbol,
                                                                bool is_testing = false);
STRATEGIES_API void DestroyMeanReverseStrategy(MeanReverseStrategy *instance);

STRATEGIES_API double MeanReverseStrategy_Execute(MeanReverseStrategy *instance,
                                                  const double *prices, size_t count,
                                                  CSVLogger &logger);

STRATEGIES_API double MeanReverseStrategy_WrapperExecute(MeanReverseStrategy *instance, size_t window_size,
                                                         const double *prices, size_t count,
                                                         CSVLogger &logger);


STRATEGIES_API BayesianSignalFiltering *CreateBayesianStrategy();

STRATEGIES_API BayesianSignalFiltering *CreateBayesianStrategyEx(const TradingParams *params, double balance,
                                                                 bool position_open, double quantity,
                                                                 double entry_price, const char *key,
                                                                 const char *secret, const char *symbol,
                                                                 bool is_testing = false);
STRATEGIES_API void DestroyBayesianStrategy(BayesianSignalFiltering *instance);

STRATEGIES_API double BayesianStrategy_Execute(BayesianSignalFiltering *instance,
                                               const double *prices, size_t count,
                                               CSVLogger &logger);

STRATEGIES_API double BayesianStrategy_WrapperExecute(BayesianSignalFiltering *instance, size_t window_size,
                                                      const double *prices, size_t count,
                                                      CSVLogger &logger);


STRATEGIES_API ArimaGarchAdaptive *CreateArimaGarchAdaptive(const double *data, size_t sz);

STRATEGIES_API ArimaGarchAdaptive *CreateArimaGarchAdaptiveEx(double *data, size_t sz, const TradingParams *params,
                                                              double balance, bool position_open,
                                                              double entry_price, double asset_quantity,
                                                              const char *key, const char *secret,
                                                              const char *symbol, bool is_testing = false);
STRATEGIES_API void DestroyArimaGarchAdaptive(ArimaGarchAdaptive *instance);

STRATEGIES_API double ArimaGarchStrategy_Execute(ArimaGarchAdaptive *instance,
                                                 const double *prices, size_t count,
                                                 CSVLogger &logger);

STRATEGIES_API double ArimaGarchStrategy_WrapperExecute(ArimaGarchAdaptive *instance, size_t window_size,
                                                        const double *prices, size_t count,
                                                        CSVLogger &logger);


STRATEGIES_API void RunInter_exchangeArbitrage(const char *crypto_pair);

STRATEGIES_API void RunIntra_exchangeArbitrage(const char **crypto_pairs, const int &version,
                                               const char *api_key, const char *secret_key);

#ifdef __cplusplus
}
#endif
