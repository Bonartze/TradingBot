#include "../include/Common.h"

auto loadCandles(const std::string &filename) -> std::vector<Candle> {
        std::vector<Candle> candles;
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
                std::stringstream ss_(line);
                std::string value;
                Candle candle = {};

                std::getline(ss_, value, ',');
                candle.timestamp = std::stol(value);
                std::getline(ss_, value, ',');
                candle.open = std::stod(value);
                std::getline(ss_, value, ',');
                candle.high = std::stod(value);
                std::getline(ss_, value, ',');
                candle.low = std::stod(value);
                std::getline(ss_, value, ',');
                candle.close = std::stod(value);
                std::getline(ss_, value, ',');
                candle.volume = std::stod(value);

                candles.push_back(candle);
        }

        Logger(LogLevel::INFO) << "Loaded candles: " << candles.size() << " from file: " << filename;
        return candles;
}

std::unordered_set<std::string> currencies = {
        "1000SATS",
        "1INCH",
        "1INCHDOWN",
        "1INCHUP",
        "1MBABYDOGE",
        "AAVE",
        "AAVEDOWN",
        "AAVEUP",
        "ACA",
        "ACE",
        "ACH",
        "ACM",
        "ACT",
        "ADA",
        "ADADOWN",
        "ADAUP",
        "ADX",
        "AE",
        "AERGO",
        "AEUR",
        "AEVO",
        "AGI",
        "AGIX",
        "AGLD",
        "AI",
        "AION",
        "AKRO",
        "ALCX",
        "ALGO",
        "ALICE",
        "ALPACA",
        "ALPHA",
        "ALPINE",
        "ALT",
        "AMB",
        "AMP",
        "ANC",
        "ANKR",
        "ANT",
        "ANY",
        "APE",
        "API3",
        "APPC",
        "APT",
        "AR",
        "ARB",
        "ARDR",
        "ARK",
        "ARKM",
        "ARN",
        "ARPA",
        "ARS",
        "ASR",
        "AST",
        "ASTR",
        "ATA",
        "ATM",
        "ATOM",
        "AUCTION",
        "AUD",
        "AUDIO",
        "AUTO",
        "AVA",
        "AVAX",
        "AXL",
        "AXS",
        "BADGER",
        "BAKE",
        "BAL",
        "BANANA",
        "BAND",
        "BAR",
        "BAT",
        "BB",
        "BCC",
        "BCD",
        "BCH",
        "BCHA",
        "BCHABC",
        "BCHDOWN",
        "BCHSV",
        "BCHUP",
        "BCN",
        "BCPT",
        "BDOT",
        "BEAM",
        "BEAMX",
        "BEAR",
        "BEL",
        "BETA",
        "BETH",
        "BGBP",
        "BICO",
        "BIDR",
        "BIFI",
        "BKRW",
        "BLUR",
        "BLZ",
        "BNB",
        "BNBBEAR",
        "BNBBULL",
        "BNBDOWN",
        "BNBUP",
        "BNSOL",
        "BNT",
        "BNX",
        "BOME",
        "BOND",
        "BONK",
        "BOT",
        "BQX",
        "BRD",
        "BRL",
        "BSW",
        "BTC",
        "BTCB",
        "BTCDOWN",
        "BTCST",
        "BTCUP",
        "BTG",
        "BTS",
        "BTT",
        "BTTC",
        "BULL",
        "BURGER",
        "BUSD",
        "BVND",
        "BZRX",
        "C98",
        "CAKE",
        "CATI",
        "CDT",
        "CELO",
        "CELR",
        "CETUS",
        "CFX",
        "CHAT",
        "CHESS",
        "CHR",
        "CHZ",
        "CITY",
        "CKB",
        "CLOAK",
        "CLV",
        "CMT",
        "CND",
        "COCOS",
        "COMBO",
        "COMP",
        "COP",
        "COS",
        "COTI",
        "COVER",
        "COW",
        "CREAM",
        "CRV",
        "CTK",
        "CTSI",
        "CTXC",
        "CVC",
        "CVP",
        "CVX",
        "CYBER",
        "CZK",
        "DAI",
        "DAR",
        "DASH",
        "DATA",
        "DCR",
        "DEGO",
        "DENT",
        "DEXE",
        "DF",
        "DGB",
        "DGD",
        "DIA",
        "DLT",
        "DNT",
        "DOCK",
        "DODO",
        "DOGE",
        "DOGS",
        "DOT",
        "DOTDOWN",
        "DOTUP",
        "DREP",
        "DUSK",
        "DYDX",
        "DYM",
        "EASY",
        "EDO",
        "EDU",
        "EGLD",
        "EIGEN",
        "ELF",
        "ENA",
        "ENG",
        "ENJ",
        "ENS",
        "EOS",
        "EOSBEAR",
        "EOSBULL",
        "EOSDOWN",
        "EOSUP",
        "EPS",
        "EPX",
        "ERD",
        "ERN",
        "ETC",
        "ETH",
        "ETHBEAR",
        "ETHBULL",
        "ETHDOWN",
        "ETHFI",
        "ETHUP",
        "EUR",
        "EURI",
        "EVX",
        "EZ",
        "FARM",
        "FDUSD",
        "FET",
        "FIDA",
        "FIL",
        "FILDOWN",
        "FILUP",
        "FIO",
        "FIRO",
        "FIS",
        "FLM",
        "FLOKI",
        "FLOW",
        "FLUX",
        "FOR",
        "FORTH",
        "FRONT",
        "FTM",
        "FTT",
        "FUEL",
        "FUN",
        "FXS",
        "G",
        "GAL",
        "GALA",
        "GAS",
        "GBP",
        "GFT",
        "GHST",
        "GLM",
        "GLMR",
        "GMT",
        "GMX",
        "GNO",
        "GNS",
        "GNT",
        "GO",
        "GRS",
        "GRT",
        "GTC",
        "GTO",
        "GVT",
        "GXS",
        "HARD",
        "HBAR",
        "HC",
        "HEGIC",
        "HFT",
        "HIFI",
        "HIGH",
        "HIVE",
        "HMSTR",
        "HNT",
        "HOOK",
        "HOT",
        "HSR",
        "ICN",
        "ICP",
        "ICX",
        "ID",
        "IDEX",
        "IDRT",
        "ILV",
        "IMX",
        "INJ",
        "INS",
        "IO",
        "IOST",
        "IOTA",
        "IOTX",
        "IQ",
        "IRIS",
        "JASMY",
        "JOE",
        "JPY",
        "JST",
        "JTO",
        "JUP",
        "JUV",
        "KAIA",
        "KAVA",
        "KDA",
        "KEEP",
        "KEY",
        "KLAY",
        "KMD",
        "KNC",
        "KP3R",
        "KSM",
        "LAZIO",
        "LDO",
        "LEND",
        "LEVER",
        "LINA",
        "LINK",
        "LINKDOWN",
        "LINKUP",
        "LISTA",
        "LIT",
        "LOKA",
        "LOOM",
        "LPT",
        "LQTY",
        "LRC",
        "LSK",
        "LTC",
        "LTCDOWN",
        "LTCUP",
        "LTO",
        "LUMIA",
        "LUN",
        "LUNA",
        "LUNC",
        "MAGIC",
        "MANA",
        "MANTA",
        "MASK",
        "MATIC",
        "MAV",
        "MBL",
        "MBOX",
        "MC",
        "MCO",
        "MDA",
        "MDT",
        "MDX",
        "MEME",
        "METIS",
        "MFT",
        "MINA",
        "MIR",
        "MITH",
        "MKR",
        "MLN",
        "MOB",
        "MOD",
        "MOVR",
        "MTH",
        "MTL",
        "MULTI",
        "MXN",
        "NANO",
        "NAS",
        "NAV",
        "NBS",
        "NCASH",
        "NEAR",
        "NEBL",
        "NEIRO",
        "NEO",
        "NEXO",
        "NFP",
        "NGN",
        "NKN",
        "NMR",
        "NOT",
        "NPXS",
        "NTRN",
        "NU",
        "NULS",
        "NXS",
        "OAX",
        "OCEAN",
        "OG",
        "OGN",
        "OM",
        "OMG",
        "OMNI",
        "ONE",
        "ONG",
        "ONT",
        "OOKI",
        "OP",
        "ORDI",
        "ORN",
        "OSMO",
        "OST",
        "OXT",
        "PAX",
        "PAXG",
        "PDA",
        "PENDLE",
        "PEOPLE",
        "PEPE",
        "PERL",
        "PERP",
        "PHA",
        "PHB",
        "PHX",
        "PIVX",
        "PIXEL",
        "PLA",
        "PLN",
        "PNT",
        "PNUT",
        "POA",
        "POE",
        "POL",
        "POLS",
        "POLY",
        "POLYX",
        "POND",
        "PORTAL",
        "PORTO",
        "POWR",
        "PPT",
        "PROM",
        "PROS",
        "PSG",
        "PUNDIX",
        "PYR",
        "PYTH",
        "QI",
        "QKC",
        "QLC",
        "QNT",
        "QSP",
        "QTUM",
        "QUICK",
        "RAD",
        "RAMP",
        "RARE",
        "RAY",
        "RCN",
        "RDN",
        "RDNT",
        "REEF",
        "REI",
        "REN",
        "RENBTC",
        "RENDER",
        "REP",
        "REQ",
        "REZ",
        "RGT",
        "RIF",
        "RLC",
        "RNDR",
        "RON",
        "RONIN",
        "ROSE",
        "RPL",
        "RPX",
        "RSR",
        "RUB",
        "RUNE",
        "RVN",
        "SAGA",
        "SALT",
        "SAND",
        "SANTOS",
        "SC",
        "SCR",
        "SCRT",
        "SEI",
        "SFP",
        "SHIB",
        "SKL",
        "SKY",
        "SLF",
        "SLP",
        "SNGLS",
        "SNM",
        "SNT",
        "SNX",
        "SOL",
        "SPARTA",
        "SPELL",
        "SRM",
        "SSV",
        "STEEM",
        "STG",
        "STMX",
        "STORJ",
        "STORM",
        "STPT",
        "STRAT",
        "STRAX",
        "STRK",
        "STX",
        "SUB",
        "SUI",
        "SUN",
        "SUPER",
        "SUSD",
        "SUSHI",
        "SUSHIDOWN",
        "SUSHIUP",
        "SWRV",
        "SXP",
        "SXPDOWN",
        "SXPUP",
        "SYN",
        "SYS",
        "T",
        "TAO",
        "TCT",
        "TFUEL",
        "THETA",
        "TIA",
        "TKO",
        "TLM",
        "TNB",
        "TNSR",
        "TNT",
        "TOMO",
        "TON",
        "TORN",
        "TRB",
        "TRIBE",
        "TRIG",
        "TROY",
        "TRU",
        "TRX",
        "TRXDOWN",
        "TRXUP",
        "TRY",
        "TURBO",
        "TUSD",
        "TUSDB",
        "TVK",
        "TWT",
        "UAH",
        "UFT",
        "UMA",
        "UNFI",
        "UNI",
        "UNIDOWN",
        "UNIUP",
        "USDC",
        "USDP",
        "USDS",
        "USDSB",
        "USDT",
        "UST",
        "USTC",
        "UTK",
        "VAI",
        "VANRY",
        "VEN",
        "VET",
        "VGX",
        "VIA",
        "VIB",
        "VIBE",
        "VIC",
        "VIDT",
        "VITE",
        "VOXEL",
        "VTHO",
        "W",
        "WABI",
        "WAN",
        "WAVES",
        "WAXP",
        "WBETH",
        "WBTC",
        "WIF",
        "WIN",
        "WING",
        "WINGS",
        "WLD",
        "WNXM",
        "WOO",
        "WPR",
        "WRX",
        "WTC",
        "XAI",
        "XEC",
        "XEM",
        "XLM",
        "XLMDOWN",
        "XLMUP",
        "XMR",
        "XNO",
        "XRP",
        "XRPBEAR",
        "XRPBULL",
        "XRPDOWN",
        "XRPUP",
        "XTZ",
        "XTZDOWN",
        "XTZUP",
        "XVG",
        "XVS",
        "XZC",
        "YFI",
        "YFIDOWN",
        "YFII",
        "YFIUP",
        "YGG",
        "YOYO",
        "ZAR",
        "ZEC",
        "ZEN",
        "ZIL",
        "ZK",
        "ZRO",
        "ZRX",
        "UNKNOWN"
};

const char *const host = "api.binance.com";
const char *const port = "443";
const char *const target = "/api/v3/ticker/price";