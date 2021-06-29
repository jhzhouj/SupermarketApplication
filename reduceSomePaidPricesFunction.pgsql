CREATE OR REPLACE FUNCTION reduceSomePaidPricesFunction(theShopperID INTEGER, numPriceReductions INTEGER) 
RETURNS INTEGER AS $$

DECLARE
    theStatus char(1);
    theCost NUMERIC (5,2);
    theProductID	INTEGER;
    updatedRowsCounter int;
    theTripTimestamp    TIMESTAMP;

    DECLARE c CURSOR FOR
       SELECT pu.tripTimeStamp, pu.productID, sh.status, pu.paidPrice 
       FROM Shoppers sh, Products pro, Purchases pu
       WHERE pro.regularPrice = pu.paidPrice
        AND sh.shopperID = pu.shopperID
        AND pro.productID = pu.productID 
        AND theShopperID = pu.shopperID
       ORDER BY pu.paidPrice DESC;
       

BEGIN
    OPEN c;
    
    updatedRowsCounter := 0;
    WHILE(numPriceReductions > 0)
    LOOP
           FETCH c INTO theTripTimeStamp, theProductID, theStatus, theCost;
           EXIT WHEN NOT FOUND;

           IF theStatus = 'H' THEN
               UPDATE Purchases p
               SET paidPrice = theCost -  2.00
               WHERE theShopperID = shopperID
               AND tripTimestamp = theTripTimeStamp
               AND productID = theProductID;
               updatedRowsCounter := updatedRowsCounter + 1;
               
           END IF;
           IF theStatus = 'M' THEN
               UPDATE Purchases 
               SET paidPrice = theCost -  1.00
               WHERE theShopperID = shopperID
               AND tripTimestamp = theTripTimeStamp
               AND productID = theProductID;
               updatedRowsCounter := updatedRowsCounter + 1;
               
           END IF;
           IF theStatus = 'L' THEN
               UPDATE Purchases 
               SET paidPrice = theCost -  0.50
               WHERE theShopperID = shopperID
               AND tripTimestamp = theTripTimeStamp
               AND productID = theProductID;
               updatedRowsCounter = updatedRowsCounter + 1;    
               
           END IF;
           
            numPriceReductions := numPriceReductions - 1;

    END LOOP;
	
	CLOSE c;
	RETURN updatedRowsCounter;




END;
$$ LANGUAGE plpgsql;
