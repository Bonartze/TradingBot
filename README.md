# Overview

This project is designed to automate a variety of trading strategies by seamlessly integrating real-time data
collection, fast strategy execution, and comprehensive analytics. The system leverages a modular architecture that
consists of several core components:

- Real-Time Data Collection:
  The DataCollector class gathers market data continuously from multiple sources, ensuring that the strategies always
  have up-to-date information to base their decisions on.


- Trading Strategies Execution:
  The platform supports multiple trading strategies such as scalping, mean reversion, arbitrage, ARIMA-GARCH, and
  Bayesian signal filtering. These strategies are implemented in C++ to maximize execution speed and efficiency. Once
  data is collected, it is immediately redirected to the corresponding strategy modules, which process it in real time
  to generate trading signals.


- Backtesting:
  The project includes some backtests proving effectiveness of the strategies


- API and Web Dashboard:
  A RESTful API, served via Node.js, provides endpoints for user authentication, strategy configuration, and retrieval
  of trading statistics. Nginx is used as a reverse proxy, ensuring secure HTTPS connections and efficient load
  distribution. The entire backend is hosted on Yandex Cloud for reliable scalability.


- Frontend Deployment:
  The interactive web dashboard, built with modern JavaScript frameworks (such as React or Vue), was deployed using
  Netlify
  for fast, scalable, and automated CI/CD. Netlify's global CDN ensures low-latency access to the trading dashboard from
  anywhere in the world.


- Metrics Visualization:
  Python is utilized to render performance metrics and visualizations, offering users an intuitive and interactive
  dashboard to monitor trading performance and key indicators in real time.

Together, these components form an end-to-end solution for automated trading, combining the speed of C++ for
high-frequency trading tasks with the flexibility of Node.js and Python for user interaction and data visualization.
The deployment stack, including Yandex Cloud for backend services and Netlify for frontend hosting, ensures high
availability, security and performance.

# Key features

- Real-Time Data Collection:
  The DataCollector class continuously gathers live market data from various sources to ensure the trading strategies
  operate on up-to-date information.


- Multiple Trading Strategies:

    - Scalping: Executes high-frequency trades based on short-term price fluctuations using configurable indicators (
      e.g., moving averages, RSI).

    - Mean Reversion: Identifies price deviations from a statistical norm and executes trades expecting a return to the
      mean.

    - ARIMA-GARCH: Combines time-series forecasting (ARIMA) with volatility modeling (GARCH) to predict and trade on
      market volatility.

    - Bayesian Signal Filtering: Uses Bayesian methods to filter market noise and generate robust trading signals.

    - Inter-Exchange & Intra-Exchange Arbitrage: Exploits price differences across or within exchanges to secure
      risk-managed profits.


- Backtesting Engine: The platform includes a comprehensive backtesting framework that allows users to simulate
  strategies on historical data, evaluate performance, and fine-tune parameters before live deployment.


- Robust API:
  A RESTful API, powered by Node.js, provides endpoints for user authentication, strategy configuration, and retrieval
  of trading statistics. All API endpoints are secured and integrated with CORS support to facilitate communication with
  the frontend dashboard.


- Secure Deployment:
  The application is deployed on Yandex Cloud with Nginx serving as a reverse proxy to handle HTTPS connections (using
  Let’s Encrypt certificates).
  Node.js and C++ components work together for a combination of flexibility and high performance.


- Interactive Metrics Visualization:
  A Python-based module renders performance metrics and charts, giving users a clear, real-time visualization of trading
  outcomes and historical performance data.

# Strategies API generation (as a Dynamic Library)

Some dependencies are required (add later)

```
git clone git@github.com:Bonartze/TradingBot.git
cd TradingBot/TradingStrategies && mkdir build && cd build
cmake ../ && make -j`nproc`
```

# Docs & UML

https://bonartze.github.io/TradingBot/

