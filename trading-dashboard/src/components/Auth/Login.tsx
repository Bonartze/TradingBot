import React, {useState} from 'react';
import {Container, Box, Typography, TextField, Button, Card, CardContent} from '@mui/material';
import {Link, useNavigate} from 'react-router-dom';
import axios from 'axios';

const Login: React.FC = () => {
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');
    const [error, setError] = useState('');
    const navigate = useNavigate();

    const handleSubmit = async (e: React.FormEvent) => {
        e.preventDefault();
        try {
            const response = await axios.post('http://89.169.163.170:5001/api/auth/login', {
                email,
                password,
            });
            const {token} = response.data;
            localStorage.setItem('token', token);
            setError('');

            navigate('/');
        } catch (err: any) {
            setError(err.response?.data?.error || 'Login error');
        }
    };

    return (
        <Container maxWidth="sm">
            <Card sx={{mt: 8, boxShadow: 3}}>
                <CardContent>
                    <Typography variant="h5" component="h2" gutterBottom>
                        Login
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
                        {error && (
                            <Typography color="error" variant="body1" sx={{mt: 1}}>
                                {error}
                            </Typography>
                        )}
                        <Button type="submit" variant="contained" color="primary" fullWidth sx={{mt: 2}}>
                            Login
                        </Button>
                        <Box textAlign="center" sx={{mt: 2}}>
                            <Typography variant="body2">
                                Don't have an account? <Link to="/register">Register</Link>
                            </Typography>
                        </Box>
                    </Box>
                </CardContent>
            </Card>
        </Container>
    );
};

export default Login;
