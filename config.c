/**
 * System configuration and initialization operations (Implementation)
 * @file: config.c
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

/** Safely free all allocated memory and exit the program
 * @param sys   global system state
 */
void safe_exit(Sys *sys){
    int i;

    for (i = 0; i < sys->unique_products; i++)
        free(sys->products_by_creation[i]);
    clear_basket(sys, BASKET_FREE);
    free_clients_tree(sys->clients_root);
    if (sys->all_invoices != NULL)
        free(sys->all_invoices);
    exit(0);
}

/** Load IVA rates from a text file
 * @param sys   global system state
 * @param argv  argument vector containing the file path
 * @return      1 if file was loaded, 0 otherwise
 */
int load_iva_from_file(Sys *sys, char *argv[]) {
    int idx, rate;
    char class;
    FILE *filep = NULL;

    filep = fopen(argv[1], "r");
    if (filep == NULL) 
        return 0;
    /* read (class number) pairs */
    while(fscanf(filep, " %c %d", &class, &rate) == 2) {
        if (class < 'A' || class > 'Z')
            continue;
        idx = class - 'A';
        if (sys->iva_table[idx] != -1 || (idx < 0 || idx >= IVAMAX))
            continue;
        sys->iva_table[idx] = rate;
    }
    fclose(filep);
    return 1;
}

/** Load default IVA rates defined in the system constants
 * @param sys   global system state
 */
void load_default_iva(Sys *sys) {
    int i;
    char classes[] = DEFAULTCLASSES;
    int rates[] = DEFAULTRATES;
    int len = sizeof(classes) / sizeof(classes[0]);

    for (i = 0; i < len; i++)
        sys->iva_table[classes[i] - 'A'] = rates[i];
}

/** Initialize the IVA table from file or defaults
 * @param sys   global system state
 * @param argc  command line argument count
 * @param argv  command line argument vector
 */
void initialize_iva(Sys *sys, int argc, char *argv[]) {
    int i;

    for(i = 0; i < IVAMAX; i++) 
        sys->iva_table[i] =  -1;
    /* load from file if provided, otherwise use defaults */
    if (argc > 1) {
        if (!load_iva_from_file(sys, argv))
            load_default_iva(sys);
    }
    else 
        load_default_iva(sys);
}

/** Initialize the global system state
 * @param sys   global system state
 * @param argc  command line argument count
 * @param argv  command line argument vector
 */
void initialize_sys(Sys *sys, int argc, char *argv[]) {
    int i;
    
    /* initialize product pointer arrays */
    for(i = 0; i < PRODMAX; i++) {
        sys->products_by_creation[i] = NULL;
        sys->products_by_ean[i] = NULL;
    }
    sys->clients_root = NULL;
    sys->all_invoices = NULL;
    /* Initialize basket */
    sys->current_basket.head = NULL;
    sys->current_basket.total_items = 0;
    sys->current_basket.unique_items = 0; 
    initialize_iva(sys, argc, argv);
    sys->n_invoices = 0;
    sys->unique_products = 0;
    sys->total_items_bought = 0;
    sys->total_invoiced_amount_in_cents = 0;
}


