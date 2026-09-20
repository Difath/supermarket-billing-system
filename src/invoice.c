/**
 * Client and invoice operations (Implementation)
 * @file:   invoice.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "../include/structs.h"
#include "../include/helpers.h"
#include "../include/config.h"
#include "../include/products.h"
#include "../include/basket.h"
#include "../include/invoice.h"

/* internal function prototypes */
int is_valid_name(char name[]);
int is_valid_nif(char nif[]);

/** Free a linked list of invoices
 * @param head  head of the invoice list
 */
void free_invoices (InvoiceNode *head){
    InvoiceNode* current = head;
    InvoiceNode* next = NULL;
    /* traverse list and free nodes */
    while (current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
}

/** Allocate and initialize a new client node
 * @param sys   global system state
 * @param name  client name
 * @return      the created client node
 */
ClientNode* create_client(Sys *sys, char name[]) {
    ClientNode *new_client;
    new_client = (ClientNode *) safe_malloc(sys, sizeof(ClientNode));
    new_client->name = (char *) safe_malloc(sys, (strlen(name) + 1));
    strcpy(new_client->name, name);
    new_client->head_invoices = NULL;
    new_client->tail_invoices = NULL;
    
    new_client->left = NULL;
    new_client->right = NULL;
    return new_client;
}

/** Add a new client to the binary search tree
 * @param sys   global system state
 * @param name  client name
 * @return      the inserted client node, or NULL
 */
ClientNode* add_client(Sys *sys, char name[]){
    ClientNode *current = sys->clients_root;
    ClientNode *new_client = create_client(sys, name); 
    int cmp;
    /* handle empty tree */
    if (current == NULL) {
        sys->clients_root = new_client;
        return new_client;
    }
    /* BST insertion logic */
    while (current != NULL) {
        cmp = strcmp(name ,current->name);
        /* navigate left if name is smaller */
        if (cmp < 0) {
            if (current->left == NULL) {
                current->left = new_client;
                return new_client;
            }
            current = current->left;
        }
        /* navigate right if name is larger or equal */
        else  {
            if (current->right == NULL){
                current->right = new_client;
                return new_client;
            }
            current = current->right;
        }
    }
    return NULL;
}

/** Allocate and initialize a new invoice node
 * @param sys       global system state
 * @param owner     client node
 * @param id        invoice ID
 * @param nif       client NIF
 * @param items_bought  total items in invoice
 * @param total_amount_paid_in_cents    total paid amount
 * @return  the created invoice node
 */
InvoiceNode* create_invoice(Sys *sys, ClientNode* owner, int id, int nif, 
    int items_bought, long total_amount_paid_in_cents){
    InvoiceNode *new_invoice;

    new_invoice = (InvoiceNode *) safe_malloc(sys, sizeof(InvoiceNode));

    new_invoice->invoice_id = id;
    new_invoice->nif = nif;
    new_invoice->items_bought = items_bought;
    new_invoice->total_amount_paid_in_cents = total_amount_paid_in_cents;
    new_invoice->owner = owner; 
    new_invoice->next = NULL;  
    return new_invoice;
}

/** Add a new invoice to the system and link it to the client
 * @param sys           global system state
 * @param owner         client node
 * @param id            invoice ID
 * @param nif           client NIF
 * @param items_bought  total items
 * @param total_paid_in_cents   total paid
 * @return  the added invoice node
 */
InvoiceNode* add_invoice(Sys *sys, ClientNode* owner, int id, int nif, 
    int items_bought, long total_paid_in_cents) {

    InvoiceNode *new_invoice;
    new_invoice = create_invoice(sys, owner, id, nif, 
        items_bought, total_paid_in_cents);
    /* resize and store the invoice pointer at index id - 1 */
    sys->all_invoices = (InvoiceNode **) safe_realloc(
        sys, sys->all_invoices, (sizeof(InvoiceNode *) * (id)));
    sys->all_invoices[id - 1] = new_invoice;
    /* update client list pointers */
    if (owner->head_invoices == NULL)
        owner->head_invoices = new_invoice;
    else
        owner->tail_invoices->next = new_invoice;
    owner->tail_invoices = new_invoice;
    /* update global counters and totals */
    sys->n_invoices += 1;
    sys->total_invoiced_amount_in_cents += total_paid_in_cents;
    return new_invoice;
}

/** Retrieve an invoice by its ID
 * @param sys           global system state
 * @param id_invoice    ID of the invoice
 * @return  the invoice node found, or NULL
 */
InvoiceNode* find_invoice(Sys *sys, int id_invoice) {
    return sys->all_invoices[id_invoice - 1];
}

/** Validate checkout inputs
 * @param nif_str       string with NIF
 * @param is_nif_alone  flag for missing name
 * @param name          client name
 * @return   1 if valid, 0 otherwise
 */
int is_valid_checkout(char nif_str[], int is_nif_alone, char name[]){
    if (is_nif_alone){
        puts(ENAME);
        return 0;
    }
    if (!is_valid_nif(nif_str)){
        printf("%s%s\n", nif_str, ENONIF);
        return 0;
    }
    if (!is_valid_name(name)){
        puts(ENAME);
        return 0;
    }
    return 1;
}

/** Validate client name format
 * @param name  client name
 * @return      1 if valid, 0 otherwise
 */
int is_valid_name(char name[]) {
    unsigned char accented_chars[] = "ÁáÀàÂâÃãÉéÈèÊêÍí"
    "ÌìÎîÓóÒòÔôÕõÚúÙùÛûÇç";
    char accented_char[3];
    int i, len_name = strlen(name), len_chars = strlen((char*)accented_chars);
    /* allowed uppercase accented characters */
    for (i = 0; i < len_name; i++){
        if (name[i] == '"')
            return 0;
    }
    /* check for standard ASCII letters */
    if (isalpha(name[0]))
        return 1;
    /* check for uppercase accented start */
    for(i = 0; i < len_chars; i += 2 ){
        /* copy 2 bytes of the UTF-8 character */
        strncpy(accented_char, (char*)&accented_chars[i], 2);
        accented_char[2] = '\0';
        if (strncmp(accented_char, name, 2) == 0)
            return 1;
    }
    return 0;
}

/** Validate NIF format
 * @param nif   NIF string
 * @return  1 if valid, 0 otherwise
 */
int is_valid_nif(char nif[] ) {
    int i, len = strlen(nif);
    if (len != 9)
        return 0;
    if (nif[0] == '0')
        return 0;
    for (i = 0; i < len; i++){
        if (!isdigit(nif[i]))
            return 0;
    }
    return 1;
}

/** Find a client in the BST by name
 * @param sys   global system state
 * @param name  client name
 * @return  client node or NULL
 */
ClientNode* find_client(Sys *sys, char name[]) {
    ClientNode *current = sys->clients_root;
    int cmp;
    if (current == NULL)
        return current;
    /* BST search traversal */
    while (current != NULL){
        cmp = strcmp(name, current->name);
        if (cmp == 0)
            return current;
        if (cmp < 0)
            current = current->left;
        else
            current = current->right;
    }
    return NULL;
}

/** Handle checkout command input
 * @param buffer    input line
 * @param name      output name
 * @param nif_str   output NIF
 * @return  1 if NIF is alone, 0 otherwise
 */
int handle_checkout_input(char *buffer, char *name, char *nif_str){
    char arg1[BUFMAX], arg2[BUFMAX];
    int is_nif_alone = 0, read_arguments;
    if (sscanf(buffer + 1, " %s", arg1) == 1) {
        if (isdigit(arg1[0])) {
            read_arguments = sscanf(buffer + 1, " %s %[^\n]", nif_str, name);
            if (read_arguments != 2)
                is_nif_alone = 1;
        }
        else if (arg1[0] == '"') {
            sscanf(buffer + 1, " %[^\n]", name); 
        }
        else {
            read_arguments = sscanf(buffer + 1, " %s %[^\n]", arg1, arg2);
            if (read_arguments == 1) {
                strcpy(name, arg1);
            }
            else {
                strcpy(nif_str, arg1);
                strcpy(name, arg2);
            }
        }
        remove_quotes(name);
    }
    return is_nif_alone;
}

/** Finalize successful checkout process
 * @param sys   global system state
 * @param nif   client NIF
 * @param name  client name
 */
void process_successful_checkout(Sys *sys, int nif, char *name){
    ClientNode *client;
    long total_paid_in_cents;
    char formated_paid_money[MONEYMAX];

    /* handle client registration */
    client = find_client(sys, name);
    if (client == NULL)
        client = add_client(sys, name);
    total_paid_in_cents = calculate_basket_total_cents(sys);
    sys->total_items_bought += sys->current_basket.total_items;
    /* add to global and client invoice records */
    add_invoice(sys, client, sys->n_invoices + 1, nif, 
        sys->current_basket.total_items, total_paid_in_cents);

    format_money(total_paid_in_cents, formated_paid_money);
    printf("%d %s %d\n", 
        sys->current_basket.total_items, formated_paid_money, sys->n_invoices);

    clear_basket(sys, BASKET_CHECKOUT);
}

/** Handle the checkout command
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_checkout(Sys *sys, char *buffer){
    int nif, is_nif_alone;
    char name[BUFMAX] = "Cliente final", nif_str[BUFMAX] = "999999999";

    is_nif_alone = handle_checkout_input(buffer, name, nif_str);
    if (!is_valid_checkout(nif_str, is_nif_alone, name)){
        return;
    }
    if (strcmp(name, "error") == 0){
        clear_basket(sys, BASKET_REVERT);
        return;
    }
    nif = atoi(nif_str);
    process_successful_checkout(sys, nif, name);
}

/** Print invoices for a specific client
 * Format "(Invoice Ids) (Value Paid) (Name)"
 * @param head  head of invoice list
 */
void print_client_invoices(InvoiceNode *head) {
    char formatted_value_paid[MONEYMAX];
    long value_paid_in_cents;
    InvoiceNode *current = head;
    while (current != NULL){
        value_paid_in_cents = current->total_amount_paid_in_cents;
        format_money(value_paid_in_cents, formatted_value_paid);
        printf("%d %s %s\n", 
            current->invoice_id,formatted_value_paid, current->owner->name);

        current = current->next;
    }
}

/** Recursively print all invoices (In-order)
 * @param root  BST root
 */
void print_all_invoices(ClientNode *root){
    if (root == NULL)
        return;
    /* in-order traversal for alphabetical listing */
    print_all_invoices(root->left);
    print_client_invoices(root->head_invoices);
    print_all_invoices(root->right);
}

/** Parse and handle the command to list invoices
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_list_invoices(Sys *sys, char *buffer){
    ClientNode *client;
    char name[BUFMAX];
    int read_arguments = sscanf(buffer + 1, " %[^\n]", name);
    /* list all if no name provided */
    if (read_arguments < 1){
        print_all_invoices(sys->clients_root);
        return;
    }
    remove_quotes(name);
    if (!is_valid_name(name)){
        puts(ENAME);
        return;
    }
    client = find_client(sys, name);
    if (client == NULL || client->head_invoices == NULL){
        printf("%s%s\n", name, ENOCLIENT);
        return;
    }
    print_client_invoices(client->head_invoices);   
}

/** Recursively free the entire client binary search tree and invoices
 * @param root  root of the client tree
 */
void free_clients_tree (ClientNode *root) {
    if (root == NULL)
        return;
    /* recursive free */
    free_clients_tree(root->left);
    free_clients_tree(root->right);

    free_invoices (root->head_invoices);
    free(root->name);
    free(root);
}