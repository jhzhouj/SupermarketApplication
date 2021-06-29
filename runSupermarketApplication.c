/**
 * runSupermarketApplication skeleton, to be modified by students
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "libpq-fe.h"

/* These constants would normally be in a header file */
/* Maximum length of string used to submit a connection */
#define  MAXCONNECTIONSTRINGSIZE    501
/* Maximum length of string used to submit a SQL statement */
#define   MAXSQLSTATEMENTSTRINGSIZE  2001

/* Exit with success after closing connection to the server
 *  and freeing memory that was used by the PGconn object.
 */
static void good_exit(PGconn *conn)
{
    PQfinish(conn);
    exit(EXIT_SUCCESS);
}

/* Exit with failure after closing connection to the server
 *  and freeing memory that was used by the PGconn object.
 */
static void bad_exit(PGconn *conn)
{
    PQfinish(conn);
    exit(EXIT_FAILURE);
}


/* The three C functions that for Lab4 should appear below.
 * Write those functions, as described in Lab4 Section 4 (and Section 5,
 * which describes the Stored Function used by the third C function).
 *
 * Write the tests of those function in main, as described in Section 6
 * of Lab4.
 */


 /* getMarketEmpCounts:
  * marketID is an attribute in the Employees table, indicating the market at
  * which the employee works.  The only argument for the getMarketEmpCounts
  * function is the database connection.
  *
  * getMarketEmpCounts should compute the number of employees who work at each
  * of the markets that has at least one employee.  getMarketEmpCounts doesn’t
  * return anything.  It should just print the number of employees who work at
  * each of the markets that has at least one employee.  Each line out by
  * getMarketEmpCounts should have the format:
  *             Market mmm has eee Employees.
  * where mmm is a market that has at least one employee and eee is the number
  * of employees who work at that market.
  */
void getMarketEmpCounts(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT m.marketID, COUNT(*) FROM Markets m, Employees e WHERE m.marketID = e.marketID GROUP BY m.marketID");

    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        bad_exit(conn);
    }


    int n = PQntuples(res);
    for (int j = 0; j < n; j++)
        printf("Market %d has %d employees\n",
                    atoi( PQgetvalue(res, j, 0) ),        // attribute 0 at row i
                    atoi( PQgetvalue(res, j, 1) ) );      // attribute 1 at row i
    printf("\n");
       
    
    PQclear(res);


}


/* updateProductManufacturer:
 * manufacturer is an attribute of Product.  Sometimes the manufacturer value
 * gets changed, e.g., if the manufacturer gets acquired.
 *
 * Besides the database connection, the updateProductManufacturer function has
 * two arguments, a string argument oldProductManufacturer and another string
 * argument, newProductManufacturer.  For every product in the Products table if
 * any) whose manufacturer equals oldProductManufacturer,
 * updateProductManufacturer should update their manufacturer to be
 * newProductManufacturer.
 *
 * There might be no Products whose manufacturer equals oldProductManufacturer
 * (that’s not an error), and there also might be multiple Products whose
 * manufacturer equals oldProductManufacturer, since manufacturer is not UNIQUE.
 * updateProductManufacturer should return the number of Products whose
 * manufacturer was updated.
 */
int updateProductManufacturer(PGconn *conn,
                              char *oldProductManufacturer,
                              char *newProductManufacturer) {

    char *query = (char*)malloc(100 * sizeof(char));
    sprintf(query, "UPDATE Products SET manufacturer = '%s' WHERE manufacturer = '%s'", newProductManufacturer, oldProductManufacturer);
    PGresult *res = PQexec(conn, query); 
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        fprintf(stderr, "UPDATE failed: %s", PQerrorMessage(conn));
        PQclear(res);
        bad_exit(conn);
    }
    int num = atoi(PQcmdTuples(res));
    PQclear(res);
    return num;

}


/* reduceSomePaidPrices:
 * Besides the database connection, this function has two integer parameters,
 * theShopperID, and numPriceReductions.  reduceSomePaidPrices invokes a Stored
 * Function, reduceSomePaidPricesFunction, that you will need to implement and
 * store in the database according to the description in Section 5.  The Stored
 * Function reduceSomePaidPricesFunction should have the same parameters,
 * theShopperID and numPriceReductions as reduceSomePaidPrices.  A value of
 * numPriceReductions that’s not positive is an error, and you should call
 * exit(EXIT_FAILURE).
 *
 * The Purchases table has attributes including shopperID (shopper who made the
 * purchase), productid (product that was purchased) and paidPrice (price that
 * was paid for the purchased product, which is not necessarily the same as the
 * regularPrice of that product).  reduceSomePaidPricesFunction will reduce the
 * paidPrice for some (but not necessarily all) of the purchases made by
 * theShopperID.  Section 5 explains which Purchases should have their paidPrice
 * reduced, and also tells you how much you should reduce those paidPrice values.
 * The reduceSomePaidPrices function should return the same integer result that
 * the reduceSomePaidPricesFunction Stored Function returns.
 *
 * The reduceSomePaidPrices function must only invoke the Stored Function
 * reduceSomePaidPricesFunction, which does all of the work for this part of the
 * assignment; reduceSomePaidPrices should not do the work itself.
 */
int reduceSomePaidPrices(PGconn *conn, int theShopperID, int numPriceReductions) {
    
    if (numPriceReductions < 0) {
        printf("Error: numPriceReductions is not positive. \n");
        exit(EXIT_FAILURE);
    }

    char *str = (char*)malloc(100 * sizeof(char));
    sprintf(str, "SELECT reduceSomePaidPricesFunction(%i, %i)", theShopperID, numPriceReductions);
    
    PGresult *res = PQexec(conn, str);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "FUNCTION failed: %s", PQerrorMessage(conn));
        PQclear(res);
        bad_exit(conn);
    }

    int num = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return num;



}

int
main(int argc, char **argv)
{
    PGconn      *conn;
    
    
    if (argc != 3) {
        fprintf(stderr, "Must supply userid and password\n");
        exit(EXIT_FAILURE);
    }
    
    char *userID = argv[1];
    char *pwd = argv[2];
    
    char conninfo[MAXCONNECTIONSTRINGSIZE] = "host=cse180-db.lt.ucsc.edu user=";
    strcat(conninfo, userID);
    strcat(conninfo, " password=");
    strcat(conninfo, pwd);
    
    /* Make a connection to the database */
    conn = PQconnectdb(conninfo);
    
    /* Check to see if the database connection was successfully made. */
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(conn));
        bad_exit(conn);
    }

        
     /* Perform the call to getMarketEmpCounts described in Section 6 of Lab4.
      * getMarketEmpCounts doesn't return anything.
      */

    printf("Output of getMarketEmpCounts:\n");
    getMarketEmpCounts(conn);



        
    /* Perform the calls to updateProductManufacturer described in Section 6
     * of Lab4, and print their outputs.
     */
    
    int cu = updateProductManufacturer(conn, "Consolidated Noodles", "Universal Pasta");
    int aa = updateProductManufacturer(conn, "Acme Coyote", "Acme Roadrunner");

    printf("Output of updateProductManufacturer when oldProductManufacturer is 'Consolidated Noodles' and newProductManufacturer is 'Universal Pasta':\n%i\n", cu);
    printf("Output of updateProductManufacturer when oldProductManufacturer is 'Acme Coyote' and newProductManufacturer is 'Acme Roadrunner':\n%i\n", aa);
     


        
    /* Perform the calls to reduceSomePaidPrices described in Section 6
     * of Lab4, and print their outputs.
     */
  
    printf("Output of reduceSomePaidPrices:\n");
    int test1 = reduceSomePaidPrices(conn, 3857, 2);
    if (test1 < 0) {
         printf("reduceSomePaidPrices when theShopperID is 3857\nand numPriceReductions is 2 had an error %d\n\n", test1);
         bad_exit(conn);
    }
    printf("reduceSomePaidPrices(%i, %i) is %i\n", 3857, 2, test1);



    int test2 = reduceSomePaidPrices(conn, 3857, 5);
    if (test2 < 0) {
         printf("reduceSomePaidPrices when theShopperID is 3857\nand numPriceReductions is 5 had an error %d\n\n", test2);
         bad_exit(conn);
    }
    printf("reduceSomePaidPrices(%i, %i) is %i\n", 3857, 5, test2);



    int test3 = reduceSomePaidPrices(conn, 2345, 3);
    if (test3 < 0) {
          printf("reduceSomePaidPrices when theShopperID is 2345\nand numPriceReductions is 3 had an error %d\n\n", test3);
          bad_exit(conn);
    }
    printf("reduceSomePaidPrices(%i, %i) is %i\n", 2345, 2, test3);



    int test4 = reduceSomePaidPrices(conn, 6228, 2);
    if (test4 < 0) {
         printf("reduceSomePaidPrices when theShopperID is 6228\nand numPriceReductions is 2 had an error %d\n\n", test4);
         bad_exit(conn);
    }
    printf("reduceSomePaidPrices(%i, %i) is %i\n", 6228, 2, test4);
    

    
    good_exit(conn);
    return 0;

}

/*
Output of getMarketEmpCounts:
Market 88 has 4 employees
Market 19 has 3 employees
Market 77 has 2 employees
Market 10 has 2 employees
Market 13 has 2 employees

Output of updateProductManufacturer when oldProductManufacturer is 'Consolidated Noodles' and newProductManufacturer is 'Universal Pasta':
4
Output of updateProductManufacturer when oldProductManufacturer is 'Acme Coyote' and newProductManufacturer is 'Acme Roadrunner':
0

Output of reduceSomePaidPrices:
reduceSomePaidPrices(3857, 2) is 2
reduceSomePaidPrices(3857, 5) is 1
reduceSomePaidPrices(2345, 2) is 2
reduceSomePaidPrices(6228, 2) is 0


*/