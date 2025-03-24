import React from 'react';
import {Routes, Route} from 'react-router-dom';
import Login from './components/Auth/Login';
import Register from './components/Auth/Register';
import Home from './components/Dashboard/Home';
import Strategies from './components/Dashboard/Strategies';
import Settings from './components/Dashboard/Settings';
import PrivateRoute from './components/Auth/PrivateRoute';
import Statistics from './components/Dashboard/Statistics';


const AppRoutes = () => (
    <Routes>
        <Route path="/login" element={<Login/>}/>
        <Route path="/register" element={<Register/>}/>
        <Route path="/statistics" element={<Statistics/>}/>


        {}
        <Route path="/" element={<Home/>}/>

        {}
        <Route
            path="/strategies"
            element={
                <PrivateRoute>
                    <Strategies/>
                </PrivateRoute>
            }
        />
        <Route
            path="/settings"
            element={
                <PrivateRoute>
                    <Settings/>
                </PrivateRoute>
            }
        />
    </Routes>
);

export default AppRoutes;
