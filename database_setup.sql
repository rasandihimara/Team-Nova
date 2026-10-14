-- Sweet Crumb Bakery database setup for MySQL Workbench
CREATE DATABASE IF NOT EXISTS sweetcrumb_bakery CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE sweetcrumb_bakery;

CREATE TABLE IF NOT EXISTS bakery_item (
  item_id VARCHAR(10) PRIMARY KEY,
  item_name VARCHAR(100) NOT NULL,
  category VARCHAR(50) NOT NULL,
  unit_price DECIMAL(10,2) NOT NULL DEFAULT 0.00,
  stock_quantity INT NOT NULL DEFAULT 0,
  reorder_level INT NOT NULL DEFAULT 0,
  CONSTRAINT chk_bakery_price CHECK (unit_price >= 0),
  CONSTRAINT chk_bakery_stock CHECK (stock_quantity >= 0),
  CONSTRAINT chk_bakery_reorder CHECK (reorder_level >= 0)
);

CREATE TABLE IF NOT EXISTS bakery_sale (
  sale_id BIGINT AUTO_INCREMENT PRIMARY KEY,
  order_number INT NOT NULL,
  sale_total DECIMAL(12,2) NOT NULL,
  sold_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO bakery_item (item_id,item_name,category,unit_price,stock_quantity,reorder_level) VALUES
('I1','Butter Cake 500g','Cakes',850.00,15,5),
('I2','Chocolate Gateau','Cakes',2200.00,8,10),
('I3','Chicken Bun','Savory',120.00,60,20),
('I4','Fish Roll','Savory',100.00,50,15),
('I5','Garlic Bread','Bread',350.00,25,10),
('I6','Chocolate Chip Cookie','Cookies',150.00,80,15)
ON DUPLICATE KEY UPDATE item_id = VALUES(item_id);
