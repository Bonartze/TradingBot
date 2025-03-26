import React from 'react';
import { Container, Typography, Box } from '@mui/material';

import homeImage from '../../images/preview.png';

const Home: React.FC = () => {
    return (
        <Box
            
            sx={{
                minHeight: '100vh',
                backgroundColor: '#f0f2f5',
                py: 4, 
            }}
        >
            <Container
                maxWidth="md"
                sx={{
                    textAlign: 'center',
                }}
            >
                <Typography variant="h3" gutterBottom>
                    Welcome to Trading Dashboard
                </Typography>
                <Typography variant="subtitle1" gutterBottom>
                    Here you can configure your strategies, view analytics, and more.
                </Typography>

                {}
                <Box
                    component="img"
                    src={homeImage}
                    alt="Trading Dashboard Preview"
                    sx={{
                        width: '80%',     
                        maxWidth: '800px',
                        borderRadius: 2,  
                        boxShadow: 3,     
                        mt: 4,            
                    }}
                />
            </Container>
        </Box>
    );
};

export default Home;
