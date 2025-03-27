import React, { useState, useEffect } from 'react';
import {
    Container,
    Typography,
    TextField,
    Button,
    Paper,
    Box,
    FormControlLabel,
    Checkbox,
    FormControl,
    InputLabel,
    Select,
    MenuItem
} from '@mui/material';
import { useSearchParams } from 'react-router-dom';
import axios from 'axios';
import DeleteAccountButton from '../Auth/DeleteAccountButton';

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

    const [email, setEmail] = useState('');
    const [windowSize, setWindowSize] = useState('10');
    const [symbol, setSymbol] = useState('BTCUSDT');
    const [dataFrame, setDataFrame] = useState('1h');
    const [isDynamic, setIsDynamic] = useState(false);
    const [balance, setBalance] = useState('1000');

    const [smaLong, setSmaLong] = useState('10');
    const [smaShort, setSmaShort] = useState('5');
    const [rsiOversold, setRsiOversold] = useState('30');
    const [rsiOverbought, setRsiOverbought] = useState('70');

    const [isTesting, setIsTesting] = useState(false);

    const [strategyInitialized, setStrategyInitialized] = useState(false);
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
            if (typeof parsed.is_testing === 'boolean') setIsTesting(parsed.is_testing);
            if (parsed.email) setEmail(parsed.email);
        }
        setStrategyInitialized(true);
    }, [strategyId]);

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
            is_testing: isTesting,
            email,
        };
        localStorage.setItem('strategySettings', JSON.stringify(strategyData));
    }, [
        strategyId,
        strategyInitialized,
        windowSize,
        symbol,
        dataFrame,
        isDynamic,
        balance,
        smaLong,
        smaShort,
        rsiOversold,
        rsiOverbought,
        isTesting,
        email,
    ]);

    const [userBgColor, setUserBgColor] = useState('#ffffff');
    const [userPassword, setUserPassword] = useState('');
    const [userInitialized, setUserInitialized] = useState(false);

    useEffect(() => {
        if (strategyId) return;
        const savedUser = localStorage.getItem('userSettings');
        if (savedUser) {
            const parsed = JSON.parse(savedUser);
            if (parsed.bgColor) setUserBgColor(parsed.bgColor);
            if (parsed.password) setUserPassword(parsed.password);
        }
        setUserInitialized(true);
    }, [strategyId]);

    useEffect(() => {
        if (strategyId || !userInitialized) return;
        const userData = {
            bgColor: userBgColor,
            password: userPassword,
        };
        localStorage.setItem('userSettings', JSON.stringify(userData));
    }, [strategyId, userInitialized, userBgColor, userPassword]);

    // Если нет strategyId, меняем цвет фона
    useEffect(() => {
        if (!strategyId) {
            document.body.style.backgroundColor = userBgColor;
        }
    }, [strategyId, userBgColor]);

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();

        const token = localStorage.getItem('token');
        if (!token) {
            alert('No token found. Please login first.');
            return;
        }

        if (strategyId) {
            const strategyName = STRATEGY_MAP[strategyId] || 'unknown strategy';

            let finalJson: any;
            // Для inter-exchange (5) и intra-exchange (6) отсутствует is_testing
            if (strategyId === '5' || strategyId === '6') {
                finalJson = {
                    strategy: strategyName,
                    symbol,
                    data_frame: dataFrame,
                    is_dynamic: isDynamic,
                    balance: Number(balance),
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
                    },
                    email
                };
            } else {
                finalJson = {
                    strategy: strategyName,
                    symbol,
                    data_frame: dataFrame,
                    is_dynamic: isDynamic,
                    balance: Number(balance),
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
                    },
                    is_testing: isTesting,
                    email
                };
            }

            console.log("Отправляем JSON (стратегия):", JSON.stringify(finalJson, null, 2));

            try {
                const response = await axios.post(
                    'https://backckkck.3utilities.com/application/json',
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
            const userJson = {
                bgColor: userBgColor,
                password: userPassword,
            };

            console.log("Отправляем JSON (пользователь):", JSON.stringify(userJson, null, 2));

            try {
                const response = await axios.post(
                    'https://backckkck.3utilities.com/application/json',
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

                            <FormControl fullWidth margin="normal" required>
                                <InputLabel>Data Frame</InputLabel>
                                <Select
                                    label="Data Frame"
                                    value={dataFrame}
                                    onChange={(e) => setDataFrame(e.target.value as string)}
                                >
                                    <MenuItem value="1m">1m</MenuItem>
                                    <MenuItem value="5m">5m</MenuItem>
                                    <MenuItem value="15m">15m</MenuItem>
                                    <MenuItem value="30m">30m</MenuItem>
                                    <MenuItem value="1h">1h</MenuItem>
                                    <MenuItem value="4h">4h</MenuItem>
                                    <MenuItem value="1d">1d</MenuItem>
                                </Select>
                            </FormControl>

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

                            <TextField
                                label="Email"
                                type="email"
                                fullWidth
                                margin="normal"
                                required
                                value={email}
                                onChange={(e) => setEmail(e.target.value)}
                            />

                            {/* Признак isTesting для стратегий, кроме 5 и 6 */}
                            {(strategyId !== '5' && strategyId !== '6') && (
                                <FormControlLabel
                                    control={
                                        <Checkbox
                                            checked={isTesting}
                                            onChange={(e) => setIsTesting(e.target.checked)}
                                        />
                                    }
                                    label="Run in Testing Mode"
                                />
                            )}

                            <Button type="submit" variant="contained" color="primary" sx={{ mt: 2 }}>
                                {strategyId ? "Run Strategy" : "Save User Settings"}
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
