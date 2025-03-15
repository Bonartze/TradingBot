import React, { useState } from 'react';
import { Container, Box, Typography, TextField, Button, Card, CardContent } from '@mui/material';
import axios from 'axios';

const Register: React.FC = () => {
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');
    const [message, setMessage] = useState('');
    const [error, setError] = useState('');

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        try {
            // Отправляем POST-запрос на ваш сервер (URL можно менять при необходимости)
            const response = await axios.post('http://localhost:5001/api/auth/register', {
                email,
                password,
            });

            // Если регистрация прошла успешно, сервер может вернуть сообщение
            setMessage(response.data.message || 'Регистрация успешна!');
            setError('');

            // При желании можно сразу перенаправлять на страницу входа
            // navigate('/login');
        } catch (err: any) {
            // Если сервер вернул ошибку, показываем её
            setMessage('');
            setError(err.response?.data?.error || 'Ошибка регистрации');
        }
    };

    return (
        <Container maxWidth="sm">
            <Card sx={{ mt: 8, boxShadow: 3 }}>
                <CardContent>
                    <Typography variant="h5" component="h2" gutterBottom>
                        Регистрация
                    </Typography>
                    <Box component="form" onSubmit={handleSubmit} sx={{ mt: 2 }}>
                        <TextField
                            label="Email"
                            type="email"
                            variant="outlined"
                            fullWidth
                            margin="normal"
                            required
                            value={email}
                            onChange={(e) => setEmail(e.target.value)}
                        />
                        <TextField
                            label="Пароль"
                            type="password"
                            variant="outlined"
                            fullWidth
                            margin="normal"
                            required
                            value={password}
                            onChange={(e) => setPassword(e.target.value)}
                        />
                        {message && (
                            <Typography color="primary" variant="body1" sx={{ mt: 1 }}>
                                {message}
                            </Typography>
                        )}
                        {error && (
                            <Typography color="error" variant="body1" sx={{ mt: 1 }}>
                                {error}
                            </Typography>
                        )}
                        <Button type="submit" variant="contained" color="primary" fullWidth sx={{ mt: 2 }}>
                            Зарегистрироваться
                        </Button>
                    </Box>
                </CardContent>
            </Card>
        </Container>
    );
};

export default Register;
