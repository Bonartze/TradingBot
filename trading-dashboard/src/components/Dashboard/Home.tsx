import React from 'react';
import { Container, Typography, Paper } from '@mui/material';

const Home: React.FC = () => {
    return (
        <Container maxWidth="md" sx={{ mt: 4 }}>
            <Paper elevation={3} sx={{ p: 3 }}>
                <Typography variant="h4" gutterBottom>
                    Добро пожаловать в Trading Dashboard
                </Typography>
                <Typography variant="body1">
                    Здесь вы можете выбрать и настроить торговые стратегии, управлять аккаунтом и отслеживать аналитику.
                </Typography>
            </Paper>
        </Container>
    );
};

export default Home;
