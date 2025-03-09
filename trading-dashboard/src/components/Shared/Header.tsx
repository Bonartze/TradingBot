import React from 'react';
import { AppBar, Toolbar, Typography, Button } from '@mui/material';
import { Link } from 'react-router-dom';

const Header: React.FC = () => (
    <AppBar position="static">
        <Toolbar>
            <Typography variant="h6" sx={{ flexGrow: 1 }}>
                Trading Dashboard
            </Typography>
            <Button color="inherit" component={Link} to="/">Home</Button>
            <Button color="inherit" component={Link} to="/strategies">Strategies</Button>
            <Button color="inherit" component={Link} to="/settings">Settings</Button>
            <Button color="inherit" component={Link} to="/login">Login</Button>
        </Toolbar>
    </AppBar>
);

export default Header;
