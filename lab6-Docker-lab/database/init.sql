CREATE DATABASE IF NOT EXISTS user_db; 
USE user_db;
CREATE TABLE IF NOT EXISTS users ( 
    user_id INT AUTO_INCREMENT PRIMARY KEY, 
    email VARCHAR(255) NOT NULL UNIQUE, 
    firstname VARCHAR(100), 
    lastname VARCHAR(100), 
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP 
    );
INSERT INTO users (email, firstname, lastname) VALUES ('admin@oslab.com', 'System', 'Admin');