/**
 * Product Invoicing System
 * This program processes user commands to manage products, 
 * shopping baskets, clients, and invoices.
 * It runs until the 'q' command is received.
 * @file: main.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "structs.h"
#include "helpers.h"
#include "config.h"     
#include "products.h" 
#include "basket.h"   
#include "invoice.h"   
#include "revenue.h"   
#include "remove.h"

/** Main execution loop
 * @param argc  argument count (used for the IVA file)
 * @param argv  argument vector
 * @return      0 on successful execution
 */
int main(int argc, char *argv[]){
    char buffer[BUFMAX];
    Sys sys;

    initialize_sys(&sys, argc, argv);
    while (fgets(buffer, BUFMAX, stdin)){	
		switch (buffer[0]) {
			case 'q': safe_exit(&sys); break;
			case 'p': cmd_add_product(&sys, buffer); break;
			case 'l': cmd_list_products(&sys, buffer); break;
			case 'a': cmd_add_basket_item(&sys, buffer); break;
			case 'r': cmd_show_revenue(&sys, buffer); break;
			case 'f': cmd_checkout(&sys, buffer); break;
			case 'd': cmd_delete(&sys, buffer); break;
			case 'c': cmd_list_invoices(&sys, buffer); break;
			default : puts(EINVALID); break;
		} 
    }
	return 0;
}