// needed to allow Visual Studio to work with scanf()
#define _CRT_SECURE_NO_WARNINGS
// carsales project.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define BOOLEAN_TRUE 1;
#define BOOLEAN_FALSE 0;

#define MAXIMUM_CARS 6 // maximum cars available for purchase hence maximum cars a customer can have.

#define MENU_HOME 99

#define LIST_AVAILABLE_CARS_OPTION 1 
#define LIST_USER_CARS_OPTION 2
#define LIST_CAR_SALES_OPTION  5
#define BUY_CAR_OPTION 3
#define CHANGE_CUSTOMER_OPTION  4
#define RESTOCK_OPTION 6
#define EXIT_OPTION 0



/**
* Checks if the current customer has at least one car purchased by
* looping through purchases vector and checking if name matches the current customer
* 
* @return BOOLEAN_TRUE if any purchase matches otherwise BOOLEAN_FALSE
*/
int hasCarsPurchased(void);

/**
* Checks if the car is in stock by acessing shopStocks array and
* see if the specific car stock is bigger than 0
*
* @param car - car index (unique identifier)
* @return BOOLEAN_TRUE if stock stock number is bigger than 0 otherwise BOOLEAN_FALSE
*/
int isCarInStock(int car);


/**
* Reads every line of sales.txt individually using fscanf() on the format specified below
* "price:carindex:customer_age:discount_given:discount_value:name_of_the_customer"
* 
* As it reads every purchase, it also subtracts from the current stock using the carindex
* 
*/
void loadData(void);

/**
* Saves all purchases to sales.txt
* 
* Loops through all items in the purchases vector and writes a line to the file using fprintf() with the format specified below
* "price:carindex:customer_age:discount_given:discount_value:name_of_the_customer"
* 
* PS: the program always assumes that the data on the file is correct
*
*/

void saveData(void);

/**
* Lists the available cars for purchase and
* asks the user to choose the car for purchase.
* 
* If the purchase is successfull a struct purchase object is generated and stored in the purchases vector.
*/
void purchaseCar(void);


/**
* Creates a sorted array based on the car stock in descending order
* Loops through every carindex on the shopStocks array and checks on many car stocks are higher
* than current carindex
* 
* @param output - pointer to int array of size MAXIMUM_CARS
*/
void getSortedCarStocksArray(int* output);

/* Converts Car name into Car's unique index number */
unsigned short getCarNumberFromName(char* name);

/* Gets the name and age of the user and tells the user if he/she is eligible to discount */
void askDetails(void);


/* Displays the list of cars not sold. */
void listCarMenu(void);

/* Displays the list of cars the user has. (if any) */
void listUserCarsMenu(void);

/* Displays the list of cars sold. */
void listCarSales(void);

/* Restocks all cars in the shopStock array */
void restock(void);

/* Gives back the memory allocated for the every single purchase */
void freePurchasesMemory(void);

/* Gets unsigned short from user while persisting for correct value */
void getUnsignedShortFromInput(unsigned short* output, char* message);

/*Gets gets string from input while persisting for correct value */
void getStringFromInput(unsigned char* output, char* message);

struct purchase {

    int discount_given; // BOOLEAN value for whether discount was used
    int discount_value; // discount total
    int price; // price of the car
    unsigned short carnumber; // unique identifier
    unsigned short customer_age; // customer age
    char name[20]; // name of the customer

};

/* Menu state control */
unsigned short MENU_STATE = MENU_HOME;

/* Shop information */
const char* shopCars[] = { "Bentley A1","Ford","Jaguar","Jeep","Land Rover","Maserati" }; // list of cars
unsigned int shopPrices[] = { 24000,13000,40000,50000,22000,1 }; // list of car prices
unsigned int shopStock[] = { 10, 9, 8, 7, 2, 5}; // original stock of all cars

/* Customer details*/
unsigned short age = 0;
char name[20];
int discount = BOOLEAN_FALSE; // eligibility for discount


vector purchases; // dynamic array for multiple purchases above the stock range (after restock)

int main(void)
{

    vector_init(&purchases);
   
    puts("Welcome to the car sales shop!");

    loadData(); // loading initial data
    askDetails(); // setups customer info

    do {

        puts("");
        puts("======== CUSTOMER ========");
        puts("1. List available Cars for purchase");
        puts("2. List customer purchases");
        puts("3. Buy Car");
        puts("4. Change customer");
        puts("======== CUSTOMER ========");
        puts("");
        puts("======== MANAGER ========");
        puts("5. View All sales");
        puts("6. Restock cars");
        puts("======== MANAGER ========");
        puts("");
        puts("0. Exit");
        puts("");

        int result = scanf("%hu", &MENU_STATE);

        
        if (MENU_STATE < 0 || MENU_STATE > 6) {
            puts("Invalid Input please try again.");
        }

        else {

            puts("");

            switch (MENU_STATE) {

                case LIST_AVAILABLE_CARS_OPTION:
                    listCarMenu();
                    break;
                case LIST_USER_CARS_OPTION:
                    listUserCarsMenu();
                    break;
                case BUY_CAR_OPTION:
                    purchaseCar();
                    break;
                case CHANGE_CUSTOMER_OPTION:
                    askDetails();
                    break;
                case LIST_CAR_SALES_OPTION:
                    listCarSales();
                    break;
                case RESTOCK_OPTION:
                    restock();
                    break;
                case EXIT_OPTION:
                    MENU_STATE = EXIT_OPTION;
                    break;
                default:
                    puts("something went wrong");
                    break;
            }
        }

        // flush buffer to disallow anomalous inputs
        while ((result = fgetc(stdin)) != '\n' && result != EOF); 

        if (MENU_STATE != EXIT_OPTION) {

            MENU_STATE = MENU_HOME;

            puts("");
            puts("Press ENTER to continue using the program.");

            getchar();
        }

        system("cls");

    } while (MENU_STATE != EXIT_OPTION);
    
    saveData();

    freePurchasesMemory();
    
    puts("Thanks for using our shop!");

    return 0;
}

int isCarInStock(int carindex) {
    return shopStock[carindex] > 0;
}
int hasCarsPurchased(void) {

    for (int i = 0; i < vector_total(&purchases); i++) {

        struct purchase * order = vector_get(&purchases, i);
        if (strcmp(order->name, name) == 0) {
            return BOOLEAN_TRUE;
        } 
    }
    return BOOLEAN_FALSE;
}

void purchaseCar(void) {

    unsigned short input = 9999;
    
    listCarMenu(); // lists again cars for the user;

    char carname[20];

    
    getStringFromInput(carname, "Which car do you want to buy?");

    input = getCarNumberFromName(carname);

    if (input == 99 ) {
        puts("Invalid car.");
        return;
    }

    if (!isCarInStock(input)) {
        puts("That car is not available.");
        return;
    }

    int price = shopPrices[input];

    if (discount) {
        price = shopPrices[input] - (shopPrices[input] * 60 / 100);
        printf("You are eligible for discount! Original price: %d  Final price: %d \n", shopPrices[input], price);
    }

    struct purchase* order = malloc(sizeof(struct purchase));

    if (order == NULL) {
        puts("Coudln't complete your order! Try again later");
        return;
    }

    
    order->discount_given = discount;
    order->discount_value = discount ? (shopPrices[input] * 60 / 100) : 0;
    order->customer_age = age;
    order->price = price;
    order->carnumber = input;
    
    strcpy(order->name, name);
    
    printf("Congratulations %s, \n", order->name);
    printf("You have just succesfully purchased a %s for %d pounds. \n", shopCars[input], price);

    vector_add(&purchases, order);

    shopStock[input] = shopStock[input] - 1;

}

void getSortedCarStocksArray(int* output) {

    int sortedResults[MAXIMUM_CARS];

    for (int index = 0; index < MAXIMUM_CARS; index++) {

        int indexpos = 0; // represents the number of stocks higher than shopStock[index]

        for (int check = 0; check < MAXIMUM_CARS; check++) {

            if (index != check && shopStock[index] <= shopStock[check]) { // sort array by shopStock
                indexpos++;
            }
        }

        /**
        * ThiS prevents the event of 2 indexes having the same indexpos
        * as there can be scenarios where N cars stock that have the same number hence
        * achieving the same indexpos.
        * Therefore it performs a loop where it looks to find the nearest empty space (aka garbage value on the array)
        */

        while (output[indexpos] >= 0 && output[indexpos] < MAXIMUM_CARS) { 
            indexpos--;
        }
        output[indexpos] = index;

    }
}

void listCarMenu(void) {

    int sortedResults[MAXIMUM_CARS];

    getSortedCarStocksArray(sortedResults);

    puts("==================================");

    for (int i = 0; i < MAXIMUM_CARS; i++) {

        int sortedCar = sortedResults[i];

        if (isCarInStock(sortedCar)) {
            puts("");
            printf("Name of the car: %s \n", shopCars[sortedCar]);
            printf("Remaining: %d \n", shopStock[sortedCar]);
            printf("Price: %d pounds \n", shopPrices[sortedCar]);
            puts("");
        }
    }
    puts("==================================");
}

void listUserCarsMenu(void) {

    if (!hasCarsPurchased()) {
  
        puts("Currently you have no cars");
        return;
    }

    puts("==================================");
    puts("Here is the list of cars you currently posess: ");

    int totalprice = 0;
    for (int i = 0; i < purchases.total; i++) {

        struct purchase* order = vector_get(&purchases, i);

        if (strcmp(order->name, name) == 0) {
            printf("           - %s   \n", shopCars[order->carnumber]);
            printf("                 %d pounds\n", order->price);
        }
        totalprice += order->price;
    }
    printf("Total money spent: %d pounds \n", totalprice);
    puts("==================================");
}


void listCarSales(void) {


    if (vector_total(&purchases) == 0) {
        printf("No sales yet.");
        return;
    }

    int sortedResults[MAXIMUM_CARS]; // holds the cars with the highest total sum in descending order
    int carTotals[MAXIMUM_CARS][2] = { 0 }; // holds the total amount of cars purchasesd for every model and price paid
    vector carBuyers[MAXIMUM_CARS] = { NULL }; // holds a dynamic char array list of buyers;


    // adding up values

    for (int i = 0; i < vector_total(&purchases); i++) {

        struct purchase* order = vector_get(&purchases, i);   
        carTotals[order->carnumber][0] = carTotals[order->carnumber][0] + order->price;
        carTotals[order->carnumber][1]++;

        vector * carbuyer = &carBuyers[order->carnumber];

        if (carbuyer->total== 0 && carbuyer->capacity == 0) {
            vector_init(carbuyer);
        }

        vector_add(carbuyer, order->name);
    }


    // sorting values (see getSortedCarStocksArray method for documentation)

    for (int index = 0; index < MAXIMUM_CARS; index++) {

        int indexpos = 0;

        for (int check = 0; check < MAXIMUM_CARS; check++) {

            if (index != check && carTotals[index][0] <= carTotals[check][0]) {
                indexpos++;
            }
        }

        while (sortedResults[indexpos] >= 0 && sortedResults[indexpos] < MAXIMUM_CARS) { //
            indexpos--;
        }

        sortedResults[indexpos] = index;

    }

    // printing

    puts("==================================");

    for (int index = 0; index < MAXIMUM_CARS; index++) {

        int sortedcar = sortedResults[index];

        if (carTotals[sortedcar][0] != 0) { // checking if there was a sale for that car

            printf("   Car: %s \n", shopCars[sortedcar]);
            puts("");
            printf("       - Total Sales: %d pounds\n" , carTotals[sortedcar][0]);
            printf("       - Total Amount of cars sold: %d \n" , carTotals[sortedcar][1]);
            printf("       - Buyers: ");

            //printing vector values
            for (int i = 0; i < vector_total(&carBuyers[sortedcar]); i++) {

                char c = (i + 1) == vector_total(&carBuyers[sortedcar]) ? ' ' : ','; // this is to check if we need to add a comma or not at the end
 
                printf("%s%c", (char *)vector_get(&carBuyers[sortedcar] , i) ,c);
            }
            puts("");
            puts("");
        }
        vector_free(&carBuyers[sortedcar]);
    }

    puts("==================================");

}

void loadData(void) {

    FILE* file;

    file = fopen("sales.txt", "r");

    if (file == NULL) {
        return;
    }
    
    struct purchase order;

    int result = 0;


    while ((result = fscanf(file, "%d:%d:%d:%hu:%hu:%19s", &order.price, &order.discount_given, &order.discount_value, &order.carnumber, &order.customer_age, order.name)) != EOF) {

        if (result == 0) {
            break;
        }

        struct purchase* orderptr = malloc(sizeof(struct purchase));

        if (orderptr != NULL) {

            orderptr->discount_given = order.discount_given;
            orderptr->discount_value = order.discount_value;
            orderptr->customer_age = order.customer_age;
            orderptr->price = order.price;
            orderptr->carnumber = order.carnumber;

            strcpy(orderptr->name, order.name);

            if (shopStock[order.carnumber] > 0) { // preventing stock from growing infinitely negative
                shopStock[order.carnumber]--;
            }
            vector_add(&purchases, orderptr);
        }
    }
    fclose(file);
}

void saveData(void) {

    FILE* file;

    file = fopen("sales.txt", "w");


    if (file == NULL) {
        return;
    }

    for (int i = 0; i < vector_total(&purchases); i++) {
        struct purchase* orderptr = vector_get(&purchases, i);
        fprintf(file, "%d:%d:%d:%hu:%hu:%19s\n", orderptr->price, orderptr->discount_given, orderptr->discount_value, orderptr->carnumber, orderptr->customer_age, orderptr->name);
    }

    fclose(file);
}

void restock(void) {
    puts("All cars have been restocked");
    shopStock[0] = 10;
    shopStock[1] = 9;
    shopStock[2] = 8;
    shopStock[3] = 7;
    shopStock[4] = 6;
    shopStock[5] = 5;
}
void askDetails(void) {

    int result = 0;

    while (result  == 0) {
        puts("What is your name?");
        result = scanf("\n%19s", &name);
    }

    result = 0;

    getUnsignedShortFromInput(&age, "What is your age?");

    if (age > 15 && age < 22) {
        puts("Our system detected that you are quite young to purchase a car!");
        puts("To encourage ambition and drive we provide a 60 discount on all cars.");
        discount = BOOLEAN_TRUE;
    }
    else if (age < 16 || age > 100) {
        puts("Too ensure safety on the roads, you are not allowed to use this program.");
        exit(0);
    }
    else {
        discount = BOOLEAN_FALSE;
    }
}
void freePurchasesMemory(void) {
    for (int i = 0; i < vector_total(&purchases); i++) {
        free(vector_get(&purchases, i));
    }
    vector_free(&purchases);
}

void getUnsignedShortFromInput(unsigned short* output, char* message) {
    
    int result = 0;

    while (result != 1) {

        puts(message);

        result = scanf("%hu", output);

        if (result != 1) {
            // flush buffer to disallow anomalous inputs
            while ((result = fgetc(stdin)) != '\n' && result != EOF);
        }

    }
}

void getStringFromInput(unsigned char* output, char* message) {

    int result = 0;

    while (result != 1) {

        puts(message);

        result = scanf("\n%[^\n]s", output);

        if (result != 1) {
            // flush buffer to disallow anomalous inputs
            while ((result = fgetc(stdin)) != '\n' && result != EOF);
        }

    }
}

unsigned short getCarNumberFromName(char* name) {

    for (int index = 0; index < MAXIMUM_CARS; index++) {

        if (strcasecmp(name, shopCars[index]) == 0) {
            return index;
        }
    }
    return 99;
}