-- Restaurant Order Management System
-- db/schema.sql
-- 7 Database Tables + Seed Data

CREATE DATABASE IF NOT EXISTS restaurant_db;
USE restaurant_db;

DROP TABLE IF EXISTS Invoice;
DROP TABLE IF EXISTS OrderItem;
DROP TABLE IF EXISTS Orders;
DROP TABLE IF EXISTS Employee;
DROP TABLE IF EXISTS Tables;
DROP TABLE IF EXISTS MenuItem;
DROP TABLE IF EXISTS Category;
DROP TABLE IF EXISTS Inventory;

-- 1. Category
CREATE TABLE Category (
    category_id INT AUTO_INCREMENT PRIMARY KEY,
    category_name VARCHAR(50) NOT NULL,
    description TEXT
);

-- 2. MenuItem
CREATE TABLE MenuItem (
    item_id INT AUTO_INCREMENT PRIMARY KEY,
    category_id INT NOT NULL,
    item_name VARCHAR(100) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    image_url VARCHAR(255),
    is_available BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (category_id) REFERENCES Category(category_id)
);

-- 3. Tables
CREATE TABLE Tables (
    table_id INT AUTO_INCREMENT PRIMARY KEY,
    table_number VARCHAR(20) NOT NULL UNIQUE,
    capacity INT DEFAULT 4,
    status VARCHAR(20) DEFAULT 'Available'
);

-- 4. Employee
CREATE TABLE Employee (
    employee_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    full_name VARCHAR(100) NOT NULL,
    role VARCHAR(20) NOT NULL,
    phone VARCHAR(20)
);

-- 5. Orders
CREATE TABLE Orders (
    order_id INT AUTO_INCREMENT PRIMARY KEY,
    table_id INT NOT NULL,
    staff_id INT,
    order_date DATETIME DEFAULT CURRENT_TIMESTAMP,
    order_status VARCHAR(20) DEFAULT 'Pending',
    total_amount DECIMAL(10,2) DEFAULT 0.00,
    FOREIGN KEY (table_id) REFERENCES Tables(table_id),
    FOREIGN KEY (staff_id) REFERENCES Employee(employee_id)
);

-- 6. OrderItem
CREATE TABLE OrderItem (
    order_item_id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT NOT NULL,
    item_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 1,
    unit_price DECIMAL(10,2) NOT NULL,
    special_note TEXT,
    item_status VARCHAR(20) DEFAULT 'Pending',
    FOREIGN KEY (order_id) REFERENCES Orders(order_id),
    FOREIGN KEY (item_id) REFERENCES MenuItem(item_id)
);

-- 7. Invoice
CREATE TABLE Invoice (
    invoice_id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT NOT NULL UNIQUE,
    cashier_id INT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    subtotal DECIMAL(10,2) NOT NULL,
    discount_amount DECIMAL(10,2) DEFAULT 0.00,
    final_amount DECIMAL(10,2) NOT NULL,
    payment_method VARCHAR(20) NOT NULL,
    payment_status VARCHAR(20) DEFAULT 'Paid',
    FOREIGN KEY (order_id) REFERENCES Orders(order_id),
    FOREIGN KEY (cashier_id) REFERENCES Employee(employee_id)
);


-- ==========================
-- SEED DATA
-- ==========================

INSERT INTO Category(category_name, description) VALUES
('Appetizers','Starter dishes'),
('Main Courses','Main food items'),
('Beverages','Drinks'),
('Desserts','Sweet dishes');

INSERT INTO MenuItem(category_id,item_name,description,price,image_url,is_available) VALUES
(1,'Spring Rolls','Fresh Vietnamese spring rolls',5.50,'spring_roll.jpg',TRUE),
(1,'French Fries','Crispy potato fries',4.00,'fries.jpg',TRUE),
(2,'Grilled Chicken','Chicken served with rice',12.50,'chicken.jpg',TRUE),
(2,'Beef Steak','Premium beef steak',18.00,'steak.jpg',TRUE),
(3,'Orange Juice','Fresh orange juice',3.50,'orange.jpg',TRUE),
(3,'Coffee','Hot coffee',2.50,'coffee.jpg',TRUE),
(4,'Ice Cream','Vanilla ice cream',4.50,'icecream.jpg',TRUE);

INSERT INTO Tables(table_number,capacity,status) VALUES
('Table 01',4,'Available'),
('Table 02',4,'Occupied'),
('Table 03',6,'Reserved'),
('Table 04',2,'Available');

INSERT INTO Employee(username,password_hash,full_name,role,phone) VALUES
('admin','hash_admin','John Manager','Admin','0900000001'),
('staff01','hash_staff','David Staff','Staff','0900000002'),
('chef01','hash_chef','Mike Chef','Chef','0900000003'),
('cashier01','hash_cashier','Anna Cashier','Cashier','0900000004');

INSERT INTO Orders(table_id,staff_id,order_status,total_amount) VALUES
(2,2,'Completed',21.00),
(1,2,'Pending',12.50);

INSERT INTO OrderItem(order_id,item_id,quantity,unit_price,special_note,item_status) VALUES
(1,3,1,12.50,'Less spicy','Served'),
(1,5,2,3.50,'No ice','Served'),
(2,4,1,18.00,'Medium rare','Pending');

INSERT INTO Invoice(order_id,cashier_id,subtotal,discount_amount,final_amount,payment_method,payment_status) VALUES
(1,4,19.50,0.00,19.50,'Cash','Paid');


-- 8. Inventory
CREATE TABLE Inventory (
    inventory_id INT AUTO_INCREMENT PRIMARY KEY,
    item_name VARCHAR(100) NOT NULL,
    quantity INT NOT NULL DEFAULT 0,
    unit VARCHAR(20) NOT NULL,
    min_stock INT DEFAULT 0,
    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP
        ON UPDATE CURRENT_TIMESTAMP
);

INSERT INTO Inventory (item_name, quantity, unit, min_stock)
VALUES
('Beef', 20, 'kg', 5),
('Chicken', 30, 'kg', 5),
('Coffee', 15, 'kg', 3),
('Orange', 50, 'piece', 10);
