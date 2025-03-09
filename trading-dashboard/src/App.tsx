import React from 'react';
import { BrowserRouter } from 'react-router-dom';
import AppRoutes from './routes';
import Header from './components/Shared/Header';

const App: React.FC = () => {
    return (
        <BrowserRouter>
            <Header />
            <AppRoutes />
        </BrowserRouter>
    );
};

export default App;
