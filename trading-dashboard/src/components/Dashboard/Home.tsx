import React from 'react';
import { Container, Typography, Paper } from '@mui/material';

const Home: React.FC = () => {
    return (
        <Container maxWidth="md" sx={{ mt: 4 }}>
            <Paper elevation={3} sx={{ p: 3 }}>
                <Typography variant="h4" gutterBottom>
                    Welcome to Trading Dashboard
                </Typography>
                <Typography variant="body1">
                    Here you can choose and configure trading strategies, manage your account, and track analytics.
                </Typography>
            </Paper>
        </Container>
    );
};

export default Home;
