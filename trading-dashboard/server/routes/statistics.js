// routes/statistics.js
const express = require('express');
const fs = require('fs');
const path = require('path');

const router = express.Router();

// 1) Эндпоинт: получить список логов по email
// Пример:
router.get('/list', (req, res) => {
    const email = req.query.email;
    console.log('[DEBUG] /list called with email=', email);

    const logsDir = path.join(__dirname, '../../../TradingEngine/Logs');
    console.log('[DEBUG] logsDir=', logsDir);

    fs.readdir(logsDir, (err, files) => {
        if (err) {
            console.error('[DEBUG] readdir error:', err);
            return res.status(500).json({error: 'Cannot read logs directory'});
        }
        console.log('[DEBUG] files:', files);

        const userLogs = files.filter(file => file.includes(email));
        console.log('[DEBUG] userLogs:', userLogs);

        return res.json({logs: userLogs});
    });
});

router.get('/file', (req, res) => {
    const filename = req.query.filename;
    if (!filename) {
        return res.status(400).json({error: 'filename is required'});
    }

    const logsDir = path.join(__dirname, '../../../TradingEngine/Logs');
    const filePath = path.join(logsDir, filename);

    if (!fs.existsSync(filePath)) {
        return res.status(404).json({error: 'File not found'});
    }

    fs.readFile(filePath, 'utf8', (err, data) => {
        if (err) {
            console.error(err);
            return res.status(500).json({error: 'Cannot read log file'});
        }

        // Возвращаем текст CSV‑файла
        return res.json({content: data});
    });
});

module.exports = router;
