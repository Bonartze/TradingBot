-- эта норм
CREATE TABLE IF NOT EXISTS Clients
(
    client_id         INTEGER PRIMARY KEY AUTOINCREMENT,
    name              TEXT        NOT NULL,
    email             TEXT UNIQUE NOT NULL,
    registration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status            TEXT      DEFAULT 'active'
);
-- это +- тоже норм, но подумать над параметрами
CREATE TABLE IF NOT EXISTS Strategies
(
    strategy_id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name          TEXT NOT NULL,
    description   TEXT,
    param_1       REAL,
    param_2       REAL,
    param_3       REAL,
    creation_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- это пиздец какая важная херня, выглядит норм, но мб потом адаптирую
CREATE TABLE IF NOT EXISTS Trades
(
    trade_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    client_id   INTEGER,
    strategy_id INTEGER,
    symbol      TEXT NOT NULL,
    trade_date  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    trade_type  TEXT CHECK (trade_type IN ('buy', 'sell')),
    amount      REAL NOT NULL,
    price       REAL NOT NULL,
    status      TEXT      DEFAULT 'pending',
    FOREIGN KEY (client_id) REFERENCES Clients (client_id),
    FOREIGN KEY (strategy_id) REFERENCES Strategies (strategy_id)
);
-- это впринципе можно убрать чуть позже
CREATE TABLE IF NOT EXISTS Logs
(
    log_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    client_id   INTEGER,
    strategy_id INTEGER,
    log_level   TEXT CHECK (log_level IN ('INFO', 'WARNING', 'ERROR')),
    message     TEXT NOT NULL,
    timestamp   TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (client_id) REFERENCES Clients (client_id),
    FOREIGN KEY (strategy_id) REFERENCES Strategies (strategy_id)
);
-- надо подумать как адаптировать
CREATE TABLE IF NOT EXISTS RiskManagement
(
    risk_id            INTEGER PRIMARY KEY AUTOINCREMENT,
    client_id          INTEGER,
    max_drawdown       REAL,
    trade_volume_limit REAL,
    leverage_limit     INTEGER,
    FOREIGN KEY (client_id) REFERENCES Clients (client_id)
);
