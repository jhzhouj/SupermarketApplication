-- Create tables for Lab4, using solution from Lab2

DROP SCHEMA Lab4 CASCADE;
CREATE SCHEMA Lab4;


-- Create tables

-- Shoppers(shopperID, shopperName, address, joinDate, status)
CREATE TABLE Shoppers (
	shopperID INTEGER, 
	shopperName VARCHAR(40), 
	address VARCHAR(60), 
	joinDate DATE,
        status CHAR(1),
	PRIMARY KEY (shopperID),
        UNIQUE (shopperName, address)
);


-- Products(productID, productName, manufacturer, regularPrice)
CREATE TABLE Products (
	productID INTEGER, 
	productName VARCHAR(40), 
	manufacturer VARCHAR(40),
        regularPrice NUMERIC(5,2) NOT NULL,
	PRIMARY KEY (productID),
        UNIQUE (productName, manufacturer)
);

 
-- Markets(marketID, address, managerID, numCheckStands)
CREATE TABLE Markets (
	marketID INTEGER,
	address VARCHAR(60),
        managerID INTEGER,
	numCheckStands INTEGER NOT NULL,
	PRIMARY KEY (marketID),
        UNIQUE (address)
);


-- Employees(empID, empName, marketID, hireDate, level, stillEmployed)
CREATE TABLE Employees (
	empID INTEGER,
        empName VARCHAR(40),
        marketID INTEGER,
	hireDate DATE,
	level CHAR(1),
	stillEmployed BOOLEAN NOT NULL,
	PRIMARY KEY (empID),
	FOREIGN KEY (marketID) REFERENCES Markets
);


-- ShoppingTrips(shopperID, tripTimestamp, marketID, empID, payType, paymentValid)
CREATE TABLE ShoppingTrips (
	shopperID INTEGER,
	tripTimestamp TIMESTAMP,
	marketID INTEGER,
        checkStand INTEGER,
        checkerID INTEGER,
	payType CHAR(1),
	paymentValid BOOLEAN,
        PRIMARY KEY (shopperID, tripTimeStamp),
	FOREIGN KEY (shopperID) REFERENCES Shoppers,
	FOREIGN KEY (marketID) REFERENCES Markets,
        FOREIGN KEY (checkerID) REFERENCES Employees(empID)       
); 


-- Purchases(shopperID,  tripTimeStamp, productID, quantity, paidPrice)
CREATE TABLE Purchases (
	shopperID INTEGER,
	tripTimestamp TIMESTAMP,
	productID INTEGER,
	quantity INTEGER,
	paidPrice NUMERIC(5,2),
        PRIMARY KEY (shopperID, tripTimeStamp, productID),
        FOREIGN KEY (shopperID, tripTimestamp) REFERENCES ShoppingTrips,
	FOREIGN KEY (productID) REFERENCES Products
);
