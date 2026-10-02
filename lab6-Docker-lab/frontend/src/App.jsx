import { useState, useEffect } from 'react' 
import './App.css' 

const API_URL = "http://localhost:3000"; 

function App() { 
    const [users, setUsers] = useState([]); 
    const [form, setForm] = useState({ email: '', firstname: '', lastname: '' }); 
    const [status, setStatus] = useState(''); 
    const fetchUsers = async () => { 
        try { 
            const res = await fetch(`${API_URL}/users`); 
            setUsers(await res.json()); 
        } catch (err) { console.error(err); } 
    };
    useEffect(() => { fetchUsers(); }, []);
    const handleSubmit = async (e) => { 
        e.preventDefault(); 
        await fetch(`${API_URL}/users`, { 
            method: 'POST', 
            headers: { 'Content-Type': 'application/json' }, 
            body: JSON.stringify(form) 
        }); 
        setForm({ email: '', firstname: '', lastname: '' }); 
        fetchUsers(); 
    };
    const handleExport = async () => { 
        const res = await fetch(`${API_URL}/save-csv`); 
        const data = await res.json(); 
        setStatus(data.message); 
    };
    return (
        <div style={{ padding: '2rem' }}> 
        <h1>OS Lab Docker Manager</h1> 
        <form onSubmit={handleSubmit}> 
            <input placeholder="Email" value={form.email} onChange={e => setForm({...form, email: e.target.value})} /> 
            <input placeholder="First Name" value={form.firstname} onChange={e => setForm({...form, firstname: e.target.value})} /> 
            <input placeholder="Last Name" value={form.lastname} onChange={e => setForm({...form, lastname: e.target.value})} /> 
            <button type="submit">Add User</button> 
            </form> 
            <button onClick={handleExport}>Save Users to CSV</button> 
            {status && <p>{status}</p>} 
            <ul> 
            {users.map(u => <li key={u.user_id}>{u.email} - {u.firstname} {u.lastname}</li>)} 
            </ul> 
            </div> 
    ) 
} 
export default App
