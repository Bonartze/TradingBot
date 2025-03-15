const express = require('express');
const bcrypt = require('bcrypt');
const jwt = require('jsonwebtoken');
const User = require('../models/User');

const router = express.Router();

router.post('/register', async (req, res) => {
    try {
        const {email, password} = req.body;
        const existingUser = await User.findOne({email});
        if (existingUser) {
            return res.status(400).json({error: 'Пользователь уже существует'});
        }

        const hashedPassword = await bcrypt.hash(password, 10);
        const newUser = new User({email, password: hashedPassword});
        await newUser.save();

        res.status(201).json({message: 'Регистрация успешна'});
    } catch (err) {
        console.error(err);
        res.status(500).json({error: 'Ошибка сервера'});
    }
});

// Вход
router.post('/login', async (req, res) => {
    try {
        const {email, password} = req.body;
        const user = await User.findOne({email});
        if (!user) {
            return res.status(400).json({error: 'Неверные учетные данные'});
        }

        const isMatch = await bcrypt.compare(password, user.password);
        if (!isMatch) {
            return res.status(400).json({error: 'Неверные учетные данные'});
        }

        const token = jwt.sign(
            {id: user._id},
            'YOUR_SECRET_KEY',
            {expiresIn: '1h'}
        );

        res.json({token});
    } catch (err) {
        console.error(err);
        res.status(500).json({error: 'Ошибка сервера'});
    }
});

module.exports = router;
