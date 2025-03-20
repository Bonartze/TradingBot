import React, {useState, useEffect} from 'react';
import {
    Container,
    Typography,
    TextField,
    Button,
    Paper,
    Box,
    FormControlLabel,
    Checkbox,
} from '@mui/material';
import {useSearchParams} from 'react-router-dom';
import axios from 'axios';
import DeleteAccountButton from '../Auth/DeleteAccountButton';

// Маппинг ID стратегии -> Название
const STRATEGY_MAP: Record<string, string> = {
    '1': 'bayesian signal pro',
    '2': 'mean reverse',
    '3': 'scalping',
    '4': 'arima garch',
    '5': 'inter-exchange arbitrage pro',
    '6': 'intra-exchange arbitrage x'
};

const Settings: React.FC = () => {
    const [searchParams] = useSearchParams();
    const strategyId = searchParams.get("strategyId");

    // ========================
    // Поля для стратегий
    // ========================
    const [windowSize, setWindowSize] = useState('10');
    const [symbol, setSymbol] = useState('BTCUSDT');
    const [dataFrame, setDataFrame] = useState('1h');
    const [isDynamic, setIsDynamic] = useState(false);
    const [balance, setBalance] = useState('1000');

    // Доп. поля для Mean Reverse/Scalping
    const [smaLong, setSmaLong] = useState('10');
    const [smaShort, setSmaShort] = useState('5');
    const [rsiOversold, setRsiOversold] = useState('30');
    const [rsiOverbought, setRsiOverbought] = useState('70');

    const [strategyInitialized, setStrategyInitialized] = useState(false);

    // Загрузка настроек стратегии из localStorage
    useEffect(() => {
        if (!strategyId) return;
        const savedStrategy = localStorage.getItem('strategySettings');
        if (savedStrategy) {
            const parsed = JSON.parse(savedStrategy);
            if (parsed.windowSize) setWindowSize(parsed.windowSize);
            if (parsed.symbol) setSymbol(parsed.symbol);
            if (parsed.dataFrame) setDataFrame(parsed.dataFrame);
            if (typeof parsed.isDynamic === 'boolean') setIsDynamic(parsed.isDynamic);
            if (parsed.balance) setBalance(parsed.balance);
            if (parsed.smaLong) setSmaLong(parsed.smaLong);
            if (parsed.smaShort) setSmaShort(parsed.smaShort);
            if (parsed.rsiOversold) setRsiOversold(parsed.rsiOversold);
            if (parsed.rsiOverbought) setRsiOverbought(parsed.rsiOverbought);
        }
        setStrategyInitialized(true);
    }, [strategyId]);

    // Сохранение настроек стратегии в localStorage
    useEffect(() => {
        if (!strategyId || !strategyInitialized) return;
        const strategyData = {
            windowSize,
            symbol,
            dataFrame,
            isDynamic,
            balance,
            smaLong,
            smaShort,
            rsiOversold,
            rsiOverbought,
        };
        localStorage.setItem('strategySettings', JSON.stringify(strategyData));
    }, [strategyId, strategyInitialized,
        windowSize, symbol, dataFrame, isDynamic, balance,
        smaLong, smaShort, rsiOversold, rsiOverbought]);

    // ========================
    // Поля для пользовательских настроек
    // ========================
    const [userBgColor, setUserBgColor] = useState('#ffffff');
    const [userPassword, setUserPassword] = useState('');
    const [userInitialized, setUserInitialized] = useState(false);

    // Загрузка userSettings
    useEffect(() => {
        if (strategyId) return; // если есть стратегия, не грузим userSettings
        const savedUser = localStorage.getItem('userSettings');
        if (savedUser) {
            const parsed = JSON.parse(savedUser);
            if (parsed.bgColor) setUserBgColor(parsed.bgColor);
            if (parsed.password) setUserPassword(parsed.password);
        }
        setUserInitialized(true);
    }, [strategyId]);

    // Сохранение userSettings
    useEffect(() => {
        if (strategyId || !userInitialized) return;
        const userData = {
            bgColor: userBgColor,
            password: userPassword,
        };
        localStorage.setItem('userSettings', JSON.stringify(userData));
    }, [strategyId, userInitialized, userBgColor, userPassword]);

    // Применение цвета фона, если нет strategyId
    useEffect(() => {
        if (!strategyId) {
            document.body.style.backgroundColor = userBgColor;
        }
    }, [strategyId, userBgColor]);

    // ========================
    // Сабмит
    // ========================
    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();

        const token = localStorage.getItem('token');
        if (!token) {
            alert('No token found. Please login first.');
            return;
        }

        // Если выбрана стратегия (strategyId != null)
        if (strategyId) {
            const strategyName = STRATEGY_MAP[strategyId] || 'unknown strategy';

            // Формируем JSON по схеме, которую ждёт сервер
            // Обратите внимание на "parameters" и "exchange_api"
            const finalJson = {
                strategy: strategyName,
                symbol: symbol,
                data_frame: dataFrame,
                is_dynamic: isDynamic,
                balance: Number(balance),  // переводим в число
                parameters: {
                    window_size: Number(windowSize),
                    sma_long: Number(smaLong),
                    sma_short: Number(smaShort),
                    rsi_oversold: Number(rsiOversold),
                    rsi_overbought: Number(rsiOverbought),
                },
                exchange_api: {
                    api_key: "YOUR_API_KEY_HERE",
                    secret_key: "YOUR_SECRET_KEY_HERE"
                }
            };

            // Выводим JSON в консоль, чтобы убедиться, что отправляем
            console.log("Отправляем JSON (стратегия):", JSON.stringify(finalJson, null, 2));

            try {
                const response = await axios.post(
                    'http://localhost:8080/application/json',
                    finalJson,
                    {
                        headers: {
                            'Content-Type': 'application/json',
                        },
                    }
                );
                alert(response.data.message || 'Strategy settings saved!');
            } catch (err: any) {
                console.log("Axios error:", err);
                alert(err.response?.data?.error || 'Error saving strategy settings');
            }

        } else {
            // Иначе сохраняем пользовательские настройки
            const userJson = {
                bgColor: userBgColor,
                password: userPassword,
            };

            // Выводим JSON в консоль
            console.log("Отправляем JSON (пользователь):", JSON.stringify(userJson, null, 2));

            try {
                const response = await axios.post(
                    'http://localhost:8080/application/json',
                    userJson,
                    {
                        headers: {
                            'Content-Type': 'application/json',
                        },
                    }
                );
                alert(response.data.message || 'User settings saved!');
            } catch (err: any) {
                console.log("Axios error:", err);
                alert(err.response?.data?.error || 'Error saving user settings');
            }
        }
    };

    return (
        <Container maxWidth="md" sx={{ mt: 4 }}>
            <Paper elevation={3} sx={{ p: 3 }}>
                {strategyId ? (
                    <>
                        <Typography variant="h4" gutterBottom>
                            Strategy Settings (ID: {strategyId})
                        </Typography>
                        <Box component="form" onSubmit={handleSubmit} noValidate sx={{ mt: 2 }}>
                            <TextField
                                label="Window Size"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                value={windowSize}
                                onChange={(e) => setWindowSize(e.target.value)}
                            />
                            <TextField
                                label="Symbol"
                                type="text"
                                fullWidth
                                margin="normal"
                                required
                                value={symbol}
                                onChange={(e) => setSymbol(e.target.value)}
                            />
                            <TextField
                                label="Data Frame"
                                type="text"
                                fullWidth
                                margin="normal"
                                required
                                value={dataFrame}
                                onChange={(e) => setDataFrame(e.target.value)}
                            />
                            <FormControlLabel
                                control={
                                    <Checkbox
                                        checked={isDynamic}
                                        onChange={(e) => setIsDynamic(e.target.checked)}
                                    />
                                }
                                label="Is Dynamic"
                            />
                            <TextField
                                label="Balance"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                value={balance}
                                onChange={(e) => setBalance(e.target.value)}
                            />
                            {(strategyId === '2' || strategyId === '3') && (
                                <>
                                    <TextField
                                        label="SMA Long"
                                        type="number"
                                        fullWidth
                                        margin="normal"
                                        required
                                        value={smaLong}
                                        onChange={(e) => setSmaLong(e.target.value)}
                                    />
                                    <TextField
                                        label="SMA Short"
                                        type="number"
                                        fullWidth
                                        margin="normal"
                                        required
                                        value={smaShort}
                                        onChange={(e) => setSmaShort(e.target.value)}
                                    />
                                    <TextField
                                        label="RSI Oversold"
                                        type="number"
                                        fullWidth
                                        margin="normal"
                                        required
                                        value={rsiOversold}
                                        onChange={(e) => setRsiOversold(e.target.value)}
                                    />
                                    <TextField
                                        label="RSI Overbought"
                                        type="number"
                                        fullWidth
                                        margin="normal"
                                        required
                                        value={rsiOverbought}
                                        onChange={(e) => setRsiOverbought(e.target.value)}
                                    />
                                </>
                            )}
                            <Button type="submit" variant="contained" color="primary" sx={{ mt: 2 }}>
                                Save Strategy Settings
                            </Button>
                        </Box>
                    </>
                ) : (
                    <>
                        <Typography variant="h4" gutterBottom>
                            User Settings
                        </Typography>
                        <Box component="form" onSubmit={handleSubmit} noValidate sx={{ mt: 2 }}>
                            <TextField
                                label="Background Color"
                                type="color"
                                fullWidth
                                margin="normal"
                                value={userBgColor}
                                onChange={(e) => setUserBgColor(e.target.value)}
                            />
                            <TextField
                                label="Password"
                                type="password"
                                fullWidth
                                margin="normal"
                                value={userPassword}
                                onChange={(e) => setUserPassword(e.target.value)}
                            />
                            <Button type="submit" variant="contained" color="primary" sx={{ mt: 2 }}>
                                Save User Settings
                            </Button>
                            <Box sx={{ mt: 4 }}>
                                <DeleteAccountButton />
                            </Box>
                        </Box>
                    </>
                )}
            </Paper>
        </Container>
    );
};

export default Settings;
