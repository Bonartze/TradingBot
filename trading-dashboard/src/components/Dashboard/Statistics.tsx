import React, { useState, useEffect } from 'react';
import axios from 'axios';
import {
    Container,
    Typography,
    List,
    ListItem,
    ListItemText,
    TextField
} from '@mui/material';

// chart.js + react-chartjs-2
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
    ChartData
} from 'chart.js';
import { Line } from 'react-chartjs-2';

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

type MyChartData = ChartData<'line'>;

const Statistics: React.FC = () => {
    // email можно получать из localStorage или из поля ввода
    const [email, setEmail] = useState('user@example.com');
    const [logFiles, setLogFiles] = useState<string[]>([]);
    const [selectedFile, setSelectedFile] = useState('');
    const [logContent, setLogContent] = useState('');
    const [chartData, setChartData] = useState<MyChartData | null>(null);

    useEffect(() => {
        if (!email) return;
        axios
            .get('http://89.169.163.170:5001/api/statistics/list', { params: { email } })
            .then(res => {
                setLogFiles(res.data.logs || []);
            })
            .catch(err => {
                console.error(err);
            });
    }, [email]);

    const loadFile = (filename: string) => {
        axios
            .get('http://89.169.163.170:5001/api/statistics/file', { params: { filename } })
            .then(res => {
                setSelectedFile(filename);
                setLogContent(res.data.content);
                // Парсим CSV
                const parsed = parseCsv(res.data.content);
                // Генерируем данные для графика
                setChartData(generateChartData(parsed));
            })
            .catch(err => {
                console.error(err);
            });
    };

    // Упрощённая функция парсинга CSV
    const parseCsv = (csvString: string) => {
        const lines = csvString.split('\n').filter(line => line.trim() !== '');
        if (lines.length === 0) {
            return { headers: [] as string[], rows: [] as string[][] };
        }
        const headers = lines[0].split(',');
        const rows = lines.slice(1).map(line => line.split(','));
        return { headers, rows };
    };

    // Генерируем данные для графика
    // Допустим, ищем колонку "Current Price" и рисуем её
    const generateChartData = (parsed: { headers: string[]; rows: string[][] }): MyChartData | null => {
        const { headers, rows } = parsed;
        if (headers.length === 0 || rows.length === 0) {
            return null;
        }

        // Ищем индекс колонки "Current Price"
        const priceIndex = headers.findIndex(h => h.trim() === 'Current Price');
        if (priceIndex < 0) {
            // Если нет колонки, вернём null
            return null;
        }

        // Преобразуем каждую строку в число
        const prices = rows.map(r => parseFloat(r[priceIndex]));
        // Для оси X используем индекс строки
        const labels = rows.map((_, i) => `Trade #${i}`);

        return {
            labels,
            datasets: [
                {
                    label: 'Current Price',
                    data: prices,
                    borderColor: 'blue',
                    backgroundColor: 'blue',
                }
            ]
        };
    };

    return (
        <Container maxWidth="md" style={{ marginTop: 40 }}>
            <Typography variant="h4" gutterBottom>
                Statistics
            </Typography>
            <Typography variant="body1" gutterBottom>
                Enter your email to load logs:
            </Typography>

            {/* Поле ввода email */}
            <TextField
                label="Email"
                variant="outlined"
                value={email}
                onChange={e => setEmail(e.target.value)}
                style={{ marginBottom: 20 }}
            />

            <Typography variant="body1">
                Below is a list of log files for {email}:
            </Typography>

            <List>
                {logFiles.map(file => (
                    <ListItem
                        key={file}
                        disablePadding
                        onClick={() => loadFile(file)}
                        style={{ cursor: 'pointer' }}
                    >
                        <ListItemText primary={file} />
                    </ListItem>
                ))}
            </List>

            {selectedFile && (
                <>
                    <Typography variant="h6" gutterBottom>
                        Selected File: {selectedFile}
                    </Typography>
                    {/* Покажем часть содержимого (или всё) */}
                    <pre style={{ maxHeight: 200, overflow: 'auto', background: '#f7f7f7' }}>
            {logContent}
          </pre>

                    {/* Если chartData сформировалось, рисуем график */}
                    {chartData && (
                        <div style={{ height: 400 }}>
                            <Line data={chartData} />
                        </div>
                    )}
                </>
            )}
        </Container>
    );
};

export default Statistics;
