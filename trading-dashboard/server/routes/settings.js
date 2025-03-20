// routes/settings.js
const express = require('express');
const router = express.Router();
const jwt = require('jsonwebtoken');

// Middleware для проверки токена
function authMiddleware(req, res, next) {
    const authHeader = req.headers.authorization;
    if (!authHeader) {
        return res.status(401).json({ error: 'No token provided' });
    }
    const token = authHeader.split(' ')[1];
    try {
        const decoded = jwt.verify(token, 'YOUR_SECRET_KEY');
        req.userId = decoded.id;
        next();
    } catch (err) {
        return res.status(401).json({ error: 'Token is not valid' });
    }
}

router.post('/', authMiddleware, async (req, res) => {
    try {
        // При желании можно сохранить настройки в БД
        console.log('Received settings:', req.body);
        return res.json({ message: 'Settings saved successfully' });
    } catch (error) {
        console.error(error);
        return res.status(500).json({ error: 'Server error' });
    }
});

module.exports = router;
