import React from 'react';
import { Routes, Route } from 'react-router-dom';
import Login from './components/Auth/Login';
import Register from './components/Auth/Register';
import Home from './components/Dashboard/Home';
import Strategies from './components/Dashboard/Strategies';
import Settings from './components/Dashboard/Settings';

const AppRoutes = () => (
    <Routes>
            <Route path="/login" element={<Login />} />
            <Route path="/register" element={<Register />} />
            <Route path="/" element={<Home />} />
            <Route path="/strategies" element={<Strategies />} />
            <Route path="/settings" element={<Settings />} />
    </Routes>
);

export default AppRoutes;
