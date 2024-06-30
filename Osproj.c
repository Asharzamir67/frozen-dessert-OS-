#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h> 
#include <unistd.h>      

#define SYS_MY_CUSTOM_LOG 335  // Define the syscall number

#define NUM_FLAVORS 5
#define NUM_TOPPINGS 5
#define MAX_CUSTOMERS 30

double pricesFlavors[NUM_FLAVORS] = {100.00, 200.00, 150.00, 180.00, 500.00};
double pricesToppings[NUM_TOPPINGS] = {90, 50, 70, 60, 30};

typedef struct {
    char name[50];
    int id;
    int flavor;
    int topping;
    double bill;
} customer;

customer customers[MAX_CUSTOMERS];
int flavors[NUM_FLAVORS] = {1, 34, 18, 25, 20};
int toppings[NUM_TOPPINGS] = {1, 34, 28, 22, 18};
double revenue = 0.0;

pthread_mutex_t flavorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t toppingMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t revenueMutex = PTHREAD_MUTEX_INITIALIZER;

void *counter_thread(void *arg);
int selectFlavor();
int selectTopping();

int main() {
    pthread_t threads[MAX_CUSTOMERS];
    int numCustomers;

    printf("Welcome to The Frozen Cream\n");
    printf("We serve ice creams and happiness!\n");
    printf("Enter Number Of Customers [1-%d]: ", MAX_CUSTOMERS);
    scanf("%d", &numCustomers);

    if (numCustomers > MAX_CUSTOMERS || numCustomers <= 0) {
        printf("Invalid Input!\n");
        return 0;
    }

    for (int i = 0; i < numCustomers; i++) {
        printf("Enter Customer %d name: ", i + 1);
        scanf("%s", customers[i].name);
        customers[i].id = i + 1;
        customers[i].flavor = selectFlavor();
        customers[i].topping = selectTopping();
    }

    for (int i = 0; i < numCustomers; i++) {
        pthread_create(&threads[i], NULL, counter_thread, (void *)&customers[i]);
    }

    for (int i = 0; i < numCustomers; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Total Revenue: RS%.2f\n", revenue);

    return 0;
}

void *counter_thread(void *arg) {
    customer *cust = (customer *)arg;
    double bill = 0.0;
    char log_msg[256];

    printf("\nCustomer %s entered the counter\n", cust->name);

    while (1) {
        pthread_mutex_lock(&flavorMutex);
        if (cust->flavor > 0 && cust->flavor <= NUM_FLAVORS && flavors[cust->flavor - 1] > 0) {
            flavors[cust->flavor - 1]--;
            pthread_mutex_unlock(&flavorMutex);
            bill += pricesFlavors[cust->flavor - 1];
            printf("\nCustomer %s chose flavor %d, costing RS%.2f\n", cust->name, cust->flavor, pricesFlavors[cust->flavor - 1]);
            break;
        } else {
            pthread_mutex_unlock(&flavorMutex);
            printf("\nSorry %s, the selected flavor is not available. choose another:\n ",cust->name);
            cust->flavor = selectFlavor();
        }
    }

    while (1) {
        pthread_mutex_lock(&toppingMutex);
        if (cust->topping > 0 && cust->topping <= NUM_TOPPINGS && toppings[cust->topping - 1] > 0) {
            toppings[cust->topping - 1]--;
            pthread_mutex_unlock(&toppingMutex);
            bill += pricesToppings[cust->topping - 1];
            printf("Customer %s added topping %d, costing RS%.2f\n", cust->name, cust->topping, pricesToppings[cust->topping - 1]);
            break;
        } else {
            pthread_mutex_unlock(&toppingMutex);
            printf("Sorry %s, the selected topping is not available. choose another:\n ",cust->name);
            cust->topping = selectTopping();
        }
    }

    sprintf(log_msg, "Customer %s - Flavor: %d, Topping: %d, Total Bill: RS%.2f", cust->name, cust->flavor, cust->topping, bill);
    syscall(SYS_MY_CUSTOM_LOG, cust->id, log_msg); // Use syscall to log message

    pthread_mutex_lock(&revenueMutex);
    revenue += bill;
    pthread_mutex_unlock(&revenueMutex);

    printf("Customer %s left the counter\n", cust->name);

    return NULL;
}


int selectFlavor() {
    int choice;
    printf("Please select a flavor [1-Vanilla, 2-Chocolate, 3-Strawberry, 4-Mint, 5-Coffee]: ");
    scanf("%d", &choice);
    printf("Selected Flavor: %d\n", choice);
    return (choice >= 1 && choice <= NUM_FLAVORS) ? choice : -1;
}

int selectTopping() {
    int choice;
    printf("Please select a topping [1-Sprinkles, 2-Chocolate chips, 3-Whipped cream, 4-Caramel sauce, 5-Nuts]: ");
    scanf("%d", &choice);
    printf("Selected Topping: %d\n", choice);
    return (choice >= 1 && choice <= NUM_TOPPINGS) ? choice : -1;
}
