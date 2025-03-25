import React, {useState} from 'react';
import {Container, Box, Typography, TextField, Button, Card, CardContent} from '@mui/material';
import axios from 'axios';
import {useNavigate} from 'react-router-dom';

const Register: React.FC = () => {
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');
    const [message, setMessage] = useState('');
    const [error, setError] = useState('');
    const navigate = useNavigate();

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        try {
            const response = await axios.post('http://89.169.163.170:5001/api/auth/register', {
                email,
                password,
            });

            setMessage(response.data.message || 'Registration successful!');
            setError('');

            // Optionally, redirect to login
            navigate('/login');
        } catch (err: any) {
            setMessage('');
            setError(err.response?.data?.error || 'Registration error');
        }
    };

    return (
        <Container maxWidth="sm">
            <Card sx={{mt: 8, boxShadow: 3}}>
                <CardContent>
                    <Typography variant="h5" component="h2" gutterBottom>
                        Register
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
                            label="Password"
                            type="password"
                            variant="outlined"
                            fullWidth
                            margin="normal"
                            required
                            value={password}
                            onChange={(e) => setPassword(e.target.value)}
                        />
                        {message && (
                            <Typography color="primary" variant="body1" sx={{mt: 1}}>
                                {message}
                            </Typography>
                        )}
                        {error && (
                            <Typography color="error" variant="body1" sx={{mt: 1}}>
                                {error}
                            </Typography>
                        )}
                        <Button type="submit" variant="contained" color="primary" fullWidth sx={{mt: 2}}>
                            Register
                        </Button>
                    </Box>
                </CardContent>
            </Card>
        </Container>
    );
};

export default Register;
