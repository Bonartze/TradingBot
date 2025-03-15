const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
const authRoutes = require('./routes/auth');

const app = express();
const PORT = process.env.PORT || 5001;

app.use(express.json());
app.use(cors());

app.use('/api/auth', authRoutes);

mongoose
    .connect('mongodb://localhost:27017/tradingbot', {
        useNewUrlParser: true,
        useUnifiedTopology: true,
    })
    .then(() => {
        console.log('Подключено к MongoDB');
        app.listen(PORT, () => console.log(`Сервер запущен на порту ${PORT}`));
    })
    .catch((err) => {
        console.error('Ошибка подключения к MongoDB:', err);
    });
