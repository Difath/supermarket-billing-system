/**
 * Shopping basket operations
 * @file: basket.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef BASKET_H
#define BASKET_H

#include "structs.h"

/** Check if a product with a specific ean is currently in the active basket
 * @param sys   global system state
 * @param ean   EAN code of the product
 * @return  1 if product is in the basket, 0 otherwise
 */
int is_product_in_basket(Sys *sys, char ean[]);

/** Clear the current basket and process items depending on the mode
 * @param sys   global system state
 * @param mode  clears for checkout, revert , or free operations
 */
void clear_basket(Sys *sys, int mode);

/** Handle the command to add items to the basket
 * @param sys   global system state
 * @param buffer    input line
 */
void cmd_add_basket_item(Sys *sys, char buffer[BUFMAX]);

/** Calculate the total cost of the current basket including taxes
 * @param sys   global system state
 * @return  total cost in cents
 */
long calculate_basket_total_cents(Sys *sys);

#endif