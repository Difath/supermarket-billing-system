/**
 * Product and invoice removal operations (Implementation)
 * @file: remove.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "structs.h"
#include "helpers.h"
#include "config.h"
#include "products.h"
#include "basket.h"
#include "invoice.h"
#include "remove.h"

/** Remove an invoice from the client's list and the global system
 * @param sys               global system state
 * @param targeted_invoice  pointer to the invoice to delete
 */
void delete_invoice(Sys *sys, InvoiceNode *targeted_invoice){ 
    ClientNode  *client =  targeted_invoice->owner;
    InvoiceNode *current = client->head_invoices;
    InvoiceNode *previous = NULL;

    /* traverse client's invoice list to unlink the node */
    while (current != NULL){
        if (current == targeted_invoice){
            /* invoice is at the head */
            if (previous == NULL){
                client->head_invoices = current->next;
                if (client->head_invoices == NULL) {
                    client->tail_invoices = NULL;
                }
            }
            /* invoice is at the tail */
            else if (current->next == NULL){
                client->tail_invoices = previous;
                previous->next = NULL;
            }
            /* invoice is in the middle */
            else
                previous->next = current->next;
            break;
        }
        previous = current;
        current = current->next;
    }
    sys->all_invoices[targeted_invoice->invoice_id - 1] = NULL;
    free(targeted_invoice);
}

/** Validate if an invoice ID can be deleted
 * @param sys           global system state
 * @param id_invoice    ID to check
 * @return              1 if valid, 0 otherwise
 */
int is_valid_delete_invoice(Sys *sys, int id_invoice){
    if (id_invoice < 1 || id_invoice > sys->n_invoices || 
    find_invoice(sys, id_invoice) == NULL){
        printf("%d%s\n", id_invoice, ENOINVOICE);
        return 0;
    }
    return 1;
}

/** Shift product pointers in an array to fill a gap
 * @param array         target pointer array
 * @param current_idx   index where the removal occurred
 * @param size          current number of elements in array
 */
void remove_product_at_index(Product **array, int current_idx, int size){
    int i;

    for(i = current_idx; i < size - 1; i++)
        array[i] = array[i + 1];
}

/** Completely remove a product from all system arrays
 * @param sys       global system state
 * @param product   pointer to the product to delete
 */
void delete_product(Sys *sys, Product *product){
    int idx_ean = find_product_idx_by_ean(sys, product->ean);
    int idx_creation =  find_product_idx_by_creation(sys, product);

    if (idx_creation == -1 && idx_ean == -1)
        return;
    free(product);
    /* update both tracking arrays */
    if (idx_creation != -1){
        remove_product_at_index(
            sys->products_by_creation, idx_creation, sys->unique_products);
    }
    if (idx_ean != -1){
        remove_product_at_index(
            sys->products_by_ean, idx_ean, sys->unique_products);
    }
}

/** Validate if a product removal operation is allowed
 * @param sys       global system state
 * @param ean       EAN code
 * @param quantity  quantity to remove
 * @return          1 if valid, 0 otherwise
 */
int is_valid_delete_product(Sys *sys, char *ean, int quantity){
    Product *product;

    if (!is_valid_ean(ean)){
        puts(EEAN);
        return 0;
    }
    if ((product = find_product(sys, ean)) == NULL){
        printf("%s%s\n", ean, ENOPRODUCT);
        return 0;
    }
    if (is_product_in_basket(sys, ean) && (product->stock - quantity <= 0)){
        puts(EPRODINUSE);
        return 0;
    }
    if (quantity <= 0 || quantity > product->stock){
        puts(EQUANTITY);
        return 0;
    }
    return 1;
}

/** Delete of an invoice and update global totals
 * @param sys           global system state
 * @param id_invoice    ID to process
 */
void handle_delete_invoice(Sys *sys, int id_invoice){
    InvoiceNode *invoice;
    char formatted_total_amount[MONEYMAX];

    if (!is_valid_delete_invoice(sys, id_invoice))
        return;
    invoice = find_invoice(sys, id_invoice);
    format_money(invoice->total_amount_paid_in_cents, formatted_total_amount);
    printf("%s %d %s\n", 
        formatted_total_amount, invoice->nif, invoice->owner->name);
    sys->total_items_bought -= invoice->items_bought;
    sys->total_invoiced_amount_in_cents -= invoice->total_amount_paid_in_cents;
    delete_invoice(sys, invoice);
}

/** Delete or reduce stock of product 
 * @param sys       global system state
 * @param ean       EAN code
 * @param quantity  quantity to remove from stock
 */
void handle_delete_product(Sys *sys, char *ean, int quantity){
    Product *product;

    if (!is_valid_delete_product(sys, ean, quantity))
        return;
    product = find_product(sys, ean);
    printf("%d %s\n", product->stock - quantity, product->description);
    /* if stock becomes zero, remove product from system */
    if ((product->stock - quantity) <= 0) {
        delete_product(sys, product);
        sys->unique_products -= 1;
    }
    else {
        product->stock -= quantity;
    }
}

/** Handle the delete command (invoices or products)
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_delete(Sys *sys, char *buffer){
    char arg1[EANMAX + 1];
    int quantity, read_arguments;
    
    read_arguments = sscanf(buffer + 1, " %14s %d", arg1, &quantity);
    if (read_arguments == 1) {
        handle_delete_invoice(sys, atoi(arg1));
    }
    else if (read_arguments == 2){
        handle_delete_product(sys, arg1, quantity);
    }
}