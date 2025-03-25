import React from 'react';
import axios from 'axios';
import {Button} from '@mui/material';
import {useNavigate} from 'react-router-dom';

const DeleteAccountButton: React.FC = () => {
    const navigate = useNavigate();

    const handleDelete = async () => {
        if (!window.confirm('Are you sure you want to delete your account? This action cannot be undone.')) {
            return;
        }
        try {
            const token = localStorage.getItem('token');
            const response = await axios.delete('https://backckkck.3utilities.com/api/auth/delete', {
                headers: {
                    Authorization: `Bearer ${token}`,
                },
            });
            alert(response.data.message);
            localStorage.removeItem('token');
            localStorage.removeItem('settings');
            navigate('/register');
        } catch (err: any) {
            alert(err.response?.data?.error || 'Error deleting account');
        }
    };

    return (
        <Button onClick={handleDelete} variant="outlined" color="error">
            Delete Account
        </Button>
    );
};

export default DeleteAccountButton;
