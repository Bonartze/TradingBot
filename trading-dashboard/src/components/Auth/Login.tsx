import React, {useState} from 'react';
import {Container, Box, Typography, TextField, Button, Card, CardContent} from '@mui/material';
import {Link} from 'react-router-dom';

const Login: React.FC = () => {
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');

    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
    };

    return (
        <Container maxWidth="sm">
            <Card sx={{mt: 8, boxShadow: 3}}>
                <CardContent>
                    <Typography variant="h5" component="h2" gutterBottom>
                        Вход в систему
                    </Typography>
                    <Box component="form" onSubmit={handleSubmit} sx={{mt: 2}}>
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
                        <Button type="submit" variant="contained" color="primary" fullWidth sx={{mt: 2}}>
                            Войти
                        </Button>
                        <Box textAlign="center" sx={{mt: 2}}>
                            <Typography variant="body2">
                                Нет аккаунта? <Link to="/register">Зарегистрируйтесь</Link>
                            </Typography>
                        </Box>
                    </Box>
                </CardContent>
            </Card>
        </Container>
    );
};

export default Login;
