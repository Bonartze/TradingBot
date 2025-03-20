import React from 'react';
import { Container, Typography, List, ListItemButton, ListItemText, Paper } from '@mui/material';
import { Link } from 'react-router-dom';

const strategies = [
    { id: 1, name: 'Bayesian Signal Pro' },
    { id: 2, name: 'Mean Reversion Master' },
    { id: 3, name: 'Scalping Elite' },
    { id: 4, name: 'ARIMA-GARCH Fusion' },
    { id: 5, name: 'Inter-Exchange Arbitrage Pro' },
    { id: 6, name: 'Intra-Exchange Arbitrage X' }
];

const Strategies: React.FC = () => {
    return (
        <Container maxWidth="md" sx={{ mt: 4 }}>
            <Paper elevation={3} sx={{ p: 3 }}>
                <Typography variant="h4" gutterBottom>
                    Select a Strategy
                </Typography>
                <List>
                    {strategies.map(strategy => (
                        <ListItemButton
                            key={strategy.id}
                            component={Link}
                            to={`/settings?strategyId=${strategy.id}`}
                        >
                            <ListItemText primary={strategy.name} />
                        </ListItemButton>
                    ))}
                </List>
            </Paper>
        </Container>
    );
};

export default Strategies;
