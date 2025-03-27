const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const fs = require('fs');
const https = require('https');

const authRoutes = require('./routes/auth');
const settingsRoutes = require('./routes/settings');
const statisticsRoutes = require('./routes/statistics');

const app = express();

const PORT = process.env.PORT || 443;

app.use(express.json());
app.use(cors({origin: '*'}));

app.use('/api/auth', authRoutes);
app.use('/api/settings', settingsRoutes);
app.use('/api/statistics', statisticsRoutes);

mongoose
    .connect('mongodb://127.0.0.1:27017/tradingbot')
    .then(() => {
        console.log('Connected to MongoDB');

        const sslOptions = {
            key: fs.readFileSync('/etc/letsencrypt/live/backckkck.3utilities.com/privkey.pem'),
            cert: fs.readFileSync('/etc/letsencrypt/live/backckkck.3utilities.com/fullchain.pem'),
        };

        https.createServer(sslOptions, app).listen(PORT, () => {
            console.log(`HTTPS server running on port ${PORT}`);
        });
    })
    .catch((err) => console.error('MongoDB connection error:', err));
