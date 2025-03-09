import React from 'react';
import { Container, Box, Typography, TextField, Button, Card, CardContent } from '@mui/material';

const Register: React.FC = () => {
    const handleSubmit = (e: React.FormEvent) => {
        e.preventDefault();
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
                        />
                        <TextField
                            label="Пароль"
                            type="password"
                            variant="outlined"
                            fullWidth
                            margin="normal"
                            required
                        />
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
