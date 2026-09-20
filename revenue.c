/**
 * Revenue and tax reporting operations (Implementation)
 * @file: revenue.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "structs.h"
#include "helpers.h"
#include "config.h"
#include "products.h"
#include "revenue.h"

/** Validate if a product's revenue can be printed
 * @param sys   global system state
 * @param ean   EAN code to check
 * @return      1 if valid, 0 otherwise
 */
int is_valid_print_product_revenue(Sys *sys, char *ean){
    Product *product;

    if (!is_valid_ean(ean)){
        puts(EEAN);
        return 0;
    }
    product = find_product(sys, ean);
    if (product == NULL){
        printf("%s%s\n", ean, ENOPRODUCT);
        return 0;
    }
    return 1;
}

/** Print sales and stock information for a specific product
 * Format "(Stock) (Amount Sold) (Description)" 
 * @param sys   global system state
 * @param ean   EAN code of the product
 */
void print_product_revenue(Sys *sys, char ean[]){
    Product *product;

    if (!is_valid_print_product_revenue(sys, ean))
        return;
    product = find_product(sys, ean);
    printf("%d %d %s\n", 
        product->stock, product->amount_sold, product->description);
}

/** Print global revenue metrics for the entire system
 * Format "(Total Items Bought) (Number of invoices) (Total revenue)"
 * @param sys   global system state
 */
void print_total_revenue(Sys *sys){
    char formated_total_invoiced[MONEYMAX];

    format_money(sys->total_invoiced_amount_in_cents, formated_total_invoiced);
    printf("%d %d %s\n", 
        sys->total_items_bought, sys->n_invoices, formated_total_invoiced);
}

/** Print all active IVA classes and their respective rates
 * Format "(Class) (Rate)"
 * @param sys   global system state
 */
void print_ivas(Sys *sys){
    char iva_class;
    int i;
    
    for(i = 0; i < IVAMAX; i++) {
        /* skip undefined classes (-1) */
        if (sys->iva_table[i] == -1)
            continue;
        iva_class = i + 'A';
        printf("%c %d%%\n",iva_class, sys->iva_table[i]);
    }
}

/** Handle the command to show revenue
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_show_revenue(Sys *sys, char buffer[BUFMAX]){
    char ean[BUFMAX]; 
    int read_arguments;

    read_arguments = sscanf(buffer + 1, " %s", ean);
    if (read_arguments == 1){
        print_product_revenue(sys, ean);
    } 
    else {
        print_total_revenue(sys);
        print_ivas(sys);
    }
}
