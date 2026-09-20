/**
 * Shopping basket operations (Implementation)
 * @file: basket.c
 * @author: ist1117887 (Diogo Monteiro)
 */

#include "structs.h"
#include "helpers.h"
#include "config.h"
#include "products.h"
#include "basket.h"

/* internal function prototypes */
BasketNode* find_basket_item(Sys *sys, Product *product);

/** Check if a product with a specific ean is currently in the active basket
 * @param sys   global system state
 * @param ean   EAN code of the product
 * @return  1 if product is in the basket, 0 otherwise
 */
int is_product_in_basket(Sys *sys, char ean[]){
    Product *product = NULL;
    BasketNode *item = NULL;

    product = find_product(sys, ean);
    if (product == NULL)
        return 0;
    item = find_basket_item(sys, product);
    if (item == NULL || item->quantity <= 0)
        return 0;
    return 1;
}

/** Allocate and initialize a new basket node for a product
 * @param sys       global system state
 * @param quantity  quantity of the product
 * @param product   product pointer
 * @return  the newly created basket node
 */
BasketNode* create_basket_item(Sys *sys,int quantity, Product *product) {
    BasketNode *new_item;

    new_item = (BasketNode *) safe_malloc(sys, sizeof(BasketNode));
    new_item->product = product;
    new_item->quantity = quantity;
    new_item->next = NULL;
    sys->current_basket.unique_items += 1;
    return new_item;
}

/** Search the basket linked list for a given product
 * @param sys       global system state
 * @param product   product pointer
 * @return  the basket node if found, NULL otherwise
 */
BasketNode* find_basket_item(Sys *sys, Product *product){
    BasketNode *search_item = sys->current_basket.head;

    while(search_item != NULL){
        if (product == search_item->product)
            return search_item;
        search_item = search_item->next;
    }
    return NULL;
}

/** Print the details of a single item in the basket
 * Format: "(Iva) (Price) (Quantity) (Taxed Price) (Description)"
 * @param sys   global system state
 * @param item  basket node to print
 */
void print_basket_item(Sys *sys, BasketNode *item) {
    Product *product = item->product;
    char formatted_taxed[MONEYMAX], formatted_price[MONEYMAX];
    long total_price, total_taxed_cents;
    int tax;

    /* retrieve tax rate for product's class */
    tax = sys->iva_table[product->iva - 'A']; 
    total_price = (long)product->price_in_cents * item->quantity;
    /* add 50 to force integer rounding */
    total_taxed_cents = (total_price * (100 + tax) + 50) / 100;
    format_money(total_taxed_cents, formatted_taxed);
    format_money(product->price_in_cents, formatted_price);
    printf("%c %s %d %s %s\n", product->iva, formatted_price,
         item->quantity, formatted_taxed, product->description);
}

/** Print all items currently in the basket
 * @param sys   global system state
 */
void print_basket(Sys *sys){
    BasketNode *item = sys->current_basket.head;
    
    /* traverse and print all items with active quantity */
    while (item != NULL){
        if (item->quantity > 0)
            print_basket_item(sys, item);
        item = item->next;
    }
}

/** Validate if the requested basket operation is allowed
 * @param sys       global system state
 * @param ean       EAN code of the product
 * @param quantity  quantity being added or removed
 * @return  1 if operation is valid, 0 otherwise
 */
int is_valid_basket_operation(Sys *sys, char ean[], int quantity){
    BasketNode *item = NULL;
    Product *product;
    int amount_on_basket = 0; 

    if (!is_valid_ean(ean)){
        puts(EEAN);
        return 0;}
    product = find_product(sys, ean);
    if (product != NULL) {
        item = find_basket_item(sys, product);
        if (item != NULL) 
            amount_on_basket = item->quantity;
    }
    /* check removal limit */
    if (quantity < 0 && (amount_on_basket + quantity) < 0){
        puts(EQUANTITY);
        return 0;}
    if (product == NULL){
        printf("%s%s\n", ean, ENOPRODUCT);
        return 0;}
    /*check stock limit*/
    if (quantity > 0 && (product->stock - quantity) < 0){
        puts(ESTOCK);
        return 0;}
    return 1;
}

/** Add a new item node to the basket list, keeping it sorted by EAN
 * @param sys       global system state
 * @param quantity  quantity of the product
 * @param product   product pointer
 * @return  the added basket node
 */
BasketNode* add_basket_item(Sys *sys, int quantity, Product *product){
    BasketNode *current = sys->current_basket.head;
    BasketNode *previous = NULL;
    BasketNode *new_item;

    new_item = create_basket_item(sys, quantity, product);
    if (sys->current_basket.head == NULL) {
        sys->current_basket.head = new_item;
        return new_item;
    }
    /* find insertion point (ascending EAN) */
    while(current != NULL &&
        is_ean_smaller(current->product->ean, product->ean)) {

        previous = current;
        current = current->next;
    }
    /* insert at the head */ 
    if (previous == NULL){
        sys->current_basket.head = new_item;
        new_item->next = current;
        return new_item;
    }
    /* insert in the middle or end of the list */
    previous->next = new_item;
    new_item->next = current;
    return new_item;
}

/** Clear the current basket and process items depending on the mode
 * @param sys   global system state
 * @param mode  clears for checkout, revert , or free operations
 */
void clear_basket(Sys *sys, int mode){
    BasketNode *current = sys->current_basket.head;
    BasketNode *next = NULL;
    Product *product;

    /* traverse basket and process items */
    while (current != NULL){
        if (mode == BASKET_REVERT) {
            product = current->product;
            product->stock += current->quantity;
            product->amount_sold -= current->quantity;
        }
        next = current->next;
        free(current);
        current = next;
    }
    sys->current_basket.head = NULL;
    sys->current_basket.total_items = 0;
    sys->current_basket.unique_items = 0;
}

/** Register an item to the basket, updating stock and sales counts
 * @param sys       global system state
 * @param product   product pointer
 * @param quantity  quantity to register
 */
void register_basket_item(Sys *sys, Product *product, int quantity) {
    BasketNode *item = find_basket_item(sys, product);

    product->amount_sold += quantity;
    product->stock -= quantity;
    sys->current_basket.total_items += quantity;
    if (item != NULL) {
        item->quantity += quantity;
    } else {
        item = add_basket_item(sys, quantity, product);
    }
    print_basket_item(sys, item);
}

/** Handle the command to add items to the basket
 * @param sys   global system state
 * @param buffer    input line
 */
void cmd_add_basket_item(Sys *sys, char buffer[BUFMAX]){
    Product *product;
    char ean[EANMAX], arg1[BUFMAX], arg2[BUFMAX];
    int quantity = 1, read_arguments;

    read_arguments = sscanf(buffer + 1, " %s %s", arg1, arg2);
    if (read_arguments <= 0){
        print_basket(sys);
        return;
    }
    if (read_arguments == 1) {
        strcpy(ean, arg1);
    }
    else if (read_arguments == 2) {
        quantity = atoi(arg1);
        strcpy(ean, arg2);
    } 
    if (!is_valid_basket_operation(sys, ean, quantity))
        return;
    
    product = find_product(sys, ean);
    register_basket_item(sys, product, quantity);
}

/** Calculate the total cost of the current basket including taxes
 * @param sys   global system state
 * @return  total cost in cents
 */
long calculate_basket_total_cents(Sys *sys) {
    int tax;
    long sum = 0, total_price;
    BasketNode *current = sys->current_basket.head;

    /* iterate basket and sum taxed items */
    while (current != NULL){
        tax = sys->iva_table[current->product->iva - 'A'];
        total_price = current->product->price_in_cents * current->quantity;
        sum += ((long) total_price * (100 + tax) + 50) / 100;
        current = current->next;
    }   
    return sum;
}