/**
 * Client and invoice operations
 * @file: invoice.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef INVOICE_H
#define INVOICE_H

#include "structs.h"

/** Finds an invoice with its id
 * @param sys           global system state
 * @param id_invoice    ID of the invoice
 * @return  the invoice node found, or NULL
 */
InvoiceNode* find_invoice(Sys *sys, int id_invoice);

/** Handle the checkout command
 * @param sys           global system state
 * @param buffer        input line
 */
void cmd_checkout(Sys *sys, char *buffer);

/** Handle the command to list invoices
 * @param sys           global system state
 * @param buffer        input line
 */
void cmd_list_invoices(Sys *sys, char *buffer);

/** Free the entire client binary search tree and invoices
 * @param root  root of the client tree
 */
void free_clients_tree(ClientNode *root);

#endif