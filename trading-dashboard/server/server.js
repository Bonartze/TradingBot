// server.js
const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const authRoutes = require('./routes/auth');
const settingsRoutes = require('./routes/settings');
const statisticsRoutes = require('./routes/statistics');



const app = express();
const PORT = 5001;

app.use(express.json());
app.use(cors());

app.use('/api/auth', authRoutes);
app.use('/api/settings', settingsRoutes);
app.use('/api/statistics', statisticsRoutes);

mongoose
    .connect('mongodb://127.0.0.1:27017/tradingbot')
    .then(() => {
        console.log('Connected to MongoDB');
        app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
    })
    .catch((err) => console.error('MongoDB connection error:', err));
