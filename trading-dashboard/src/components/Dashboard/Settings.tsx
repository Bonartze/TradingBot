import React from 'react';
import { Container, Typography, TextField, Button, Paper, Box } from '@mui/material';
import { useSearchParams } from 'react-router-dom';

const Settings: React.FC = () => {
    const [searchParams] = useSearchParams();
    const strategyId = searchParams.get("strategyId");

    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
    };

    return (
        <Container maxWidth="md" sx={{ mt: 4 }}>
            <Paper elevation={3} sx={{ p: 3 }}>
                <Typography variant="h4" gutterBottom>
                    Настройки
                </Typography>
                {strategyId && (
                    <Typography variant="h6" gutterBottom>
                        Настройка стратегии ID: {strategyId}
                    </Typography>
                )}
                <Box component="form" onSubmit={handleSubmit} noValidate sx={{ mt: 2 }}>
                    {/* Пример для Bayesian Signal Filtering */}
                    {strategyId === "1" && (
                        <>
                            <TextField
                                label="Верхняя граница вероятности"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                            <TextField
                                label="Нижняя граница вероятности"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                        </>
                    )}
                    {/* Пример для Mean Reversion */}
                    {strategyId === "2" && (
                        <>
                            <TextField
                                label="SMA Short"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={5}
                            />
                            <TextField
                                label="SMA Long"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={10}
                            />
                            <TextField
                                label="RSI Oversold"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={30}
                            />
                            <TextField
                                label="RSI Overbought"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={29}
                            />
                        </>
                    )}
                    {/* Пример для Scalping */}
                    {strategyId === "3" && (
                        <>
                            <TextField
                                label="Scalping SMA Short"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={5.0}
                            />
                            <TextField
                                label="Scalping SMA Long"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={10.0}
                            />
                            <TextField
                                label="Scalping RSI Oversold"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={30.0}
                            />
                            <TextField
                                label="Scalping RSI Overbought"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                                defaultValue={29.0}
                            />
                        </>
                    )}
                    {/* Пример для ARIMA-GARCH */}
                    {strategyId === "4" && (
                        <>
                            <TextField
                                label="ARIMA Order (p)"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                            <TextField
                                label="Differencing (d)"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                            <TextField
                                label="GARCH Alpha"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                            <TextField
                                label="GARCH Beta"
                                type="number"
                                fullWidth
                                margin="normal"
                                required
                            />
                        </>
                    )}
                    {/* Пример для Арбитражных стратегий */}
                    {strategyId === "5" && (
                        <>
                            <TextField
                                label="Торговые пары для арбитража"
                                type="text"
                                fullWidth
                                margin="normal"
                                required
                                helperText="Например: BTC/USDT, ETH/USDT, LTC/BTC"
                            />
                        </>
                    )}
                    {strategyId === "6" && (
                        <>
                            <TextField
                                label="Торговые пары для внутрибиржевого арбитража"
                                type="text"
                                fullWidth
                                margin="normal"
                                required
                                helperText="Например: BTC/USDT, ETH/USDT"
                            />
                        </>
                    )}

                    <Typography variant="h5" sx={{ mt: 3 }}>
                        API-ключи
                    </Typography>
                    <TextField
                        label="Binance API Key"
                        type="text"
                        fullWidth
                        margin="normal"
                        required
                    />
                    <TextField
                        label="Kraken API Key"
                        type="text"
                        fullWidth
                        margin="normal"
                        required
                    />
                    <TextField
                        label="OKX API Key"
                        type="text"
                        fullWidth
                        margin="normal"
                        required
                    />

                    <Typography variant="h5" sx={{ mt: 3 }}>
                        Персональные настройки
                    </Typography>
                    <TextField
                        label="Изменить фон"
                        type="color"
                        fullWidth
                        margin="normal"
                    />
                    <TextField
                        label="Сменить пароль"
                        type="password"
                        fullWidth
                        margin="normal"
                    />

                    <Button type="submit" variant="contained" color="primary" sx={{ mt: 2 }}>
                        Сохранить настройки
                    </Button>
                </Box>
            </Paper>
        </Container>
    );
};

export default Settings;
