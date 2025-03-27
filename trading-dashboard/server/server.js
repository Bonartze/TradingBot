const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const https = require('https');
const fs = require('fs');

const authRoutes = require('./routes/auth');
const settingsRoutes = require('./routes/settings');
const statisticsRoutes = require('./routes/statistics');

const app = express();

const HTTP_PORT = process.env.PORT || 5001;
const HTTPS_PORT = 443;

const sslOptions = {
    key:  fs.readFileSync('/etc/letsencrypt/live/backckkck.3utilities.com/privkey.pem'),
    cert: fs.readFileSync('/etc/letsencrypt/live/backckkck.3utilities.com/fullchain.pem')
};

app.use(express.json());
app.use(cors({ origin: '*' }));

app.use('/api/auth', authRoutes);
app.use('/api/settings', settingsRoutes);
app.use('/api/statistics', statisticsRoutes);

mongoose
    .connect('mongodb://127.0.0.1:27017/tradingbot')
    .then(() => {
        console.log('Connected to MongoDB');

        app.listen(HTTP_PORT, '0.0.0.0', () => {
            console.log(`HTTP server running on port ${HTTP_PORT}`);

            https.createServer(sslOptions, app).listen(HTTPS_PORT, () => {
                console.log(`HTTPS server running on port ${HTTPS_PORT}`);
            });
        });
    })
    .catch(err => {
        console.error('MongoDB connection error:', err);
    });
