/**
 * Product operations (Implementation)
 * @file: products.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "structs.h"
#include "helpers.h"
#include "config.h"
#include "products.h"
#include "basket.h"

/** Compare two EAN strings alphabetically
 * @param ean1  first EAN
 * @param ean2  second EAN
 * @return  1 if ean1 < ean2, 0 otherwise
 */
int is_ean_smaller(char ean1[], char ean2[]){
    return (strcmp(ean1, ean2) < 0);
}

/** Validate EAN-13 or EAN-8 format and check digit
 * @param ean   EAN string to validate
 * @return  1 if valid, 0 otherwise
 */
int is_valid_ean(char ean[EANMAX]) {
    int len = strlen(ean); 
    int i, digit, control_digit, sum = 0;

    if (len != LENEAN13 && len != LENEAN8)
        return 0;
    /* Calculate digit sum with EAN weights (1 and 3) */
    for (i = 0; i < len - 1; i++) {
        if (!isdigit(ean[i])) 
            return 0;
        digit = ean[i] - '0';
        sum += (i % 2 == 0) ? digit : (digit*3);
    }
    /* Verify control digit */
    control_digit = ean[len-1] - '0';
    if (control_digit != (10- (sum % 10)) %10)
        return 0;
    return 1;
}

/** Check if an IVA class is valid and exists in the system
 * @param sys           global system state
 * @param iva_class     IVA class character
 * @return  1 if valid, 0 otherwise
 */
int is_valid_iva(Sys *sys, char iva_class){
    if (iva_class < 'A' || iva_class > 'Z')
        return 0;
    if (sys->iva_table[iva_class - 'A'] == -1)
        return 0;
    return 1; 
}

/** Validate if a description is valid 
 * (starts with uppercase or accented uppercase)
 * @param description   product description string
 * @return  1 if valid, 0 otherwise
 */
int is_valid_description(char description[DESCRIPTIONMAX]){
    unsigned char uppercase_accents[] = "ÁÀÂÃÇÉÊÍÓÔÕÚ";
    char accented_char[3];
    int i, len_description = strlen(description);
    int len_chars = strlen((char*)uppercase_accents);

    if (len_description == 0 || len_description > DESCRIPTIONMAX -1)
        return 0;
    if (description[0] >= 'A' && description[0] <= 'Z')
        return 1;
    /* handle multi-byte characters */
    for (i = 0; i < len_chars; i += 2){
        strncpy(accented_char, (char*)&uppercase_accents[i], 2);
        accented_char[2] = '\0';
        if (strncmp(accented_char, description, 2) == 0)
            return 1;
    }
    return 0;
}

/** Check if the price of an existing product has changed
 * @param sys               global system state
 * @param ean               EAN code
 * @param price_in_cents    new price to compare
 * @return  1 if changed, 0 otherwise
 */
int has_price_changed(Sys *sys, char ean[], long price_in_cents){
    Product *product = find_product(sys, ean);

    if (product == NULL)
        return 0;
    return (price_in_cents != product->price_in_cents);
}

/** Validate all product data
 * @param sys           global system state
 * @param ean           EAN code
 * @param iva_class     IVA class
 * @param price         price in euros
 * @param quantity      stock quantity
 * @param description   product description
 * @return  1 if all data is valid, 0 otherwise
 */
int is_valid_product_data(Sys *sys, char ean[], char iva_class,
    double price, int quantity, char description[]){
        
    if (!is_valid_ean(ean)) {
        puts(EEAN);
        return 0; }
    if (!is_valid_iva(sys, iva_class)) {
        puts(EIVA);
        return 0; }
    if (euros_to_cents(price) <= 0) {
        puts(EPRICE);
        return 0; }
    if (quantity < 0) {
        puts(EQUANTITY);
        return 0; }
    if (!is_valid_description(description)) {
        puts(EDESCRIPTION);
        return 0; }
    if(is_product_in_basket(sys, ean) && 
    has_price_changed(sys, ean, euros_to_cents(price))){
        puts(EPRODINUSE);
        return 0; }
    if (find_product(sys, ean) == NULL && sys->unique_products >= PRODMAX) {
        puts(EPRODUCT);
        return 0; }
    return 1;
}

/** Update fields of an existing product
 * @param product           pointer to existing product
 * @param iva_class         new IVA class
 * @param price_in_cents    new price
 * @param quantity          stock to add
 * @param description       new description
 */
void update_product(Product *product, char iva_class, 
    long price_in_cents , int quantity, char description[]){

    product->iva = iva_class;
    product->price_in_cents = price_in_cents;
    product->stock += quantity;
    strcpy(product->description, description);
}

/** Find a product index in the EAN sorted array using binary search
 * @param sys   global system state
 * @param ean   EAN code to search
 * @return  index in the array, or -1 if not found
 */
int find_product_idx_by_ean(Sys *sys, char *ean){
    int low = 0, mid, high = sys->unique_products - 1;
    int cmp;

    /* binary search logic */
    while (low <= high){
        mid = low + ((high - low) / 2);
        cmp = strcmp(ean, sys->products_by_ean[mid]->ean);
        if (cmp == 0)
            return mid;
        if (cmp < 0)
            high = mid - 1;
        else
            low = mid + 1;
        }
    return -1;
}

/** Find a product index in the creation order array
 * @param sys       global system state
 * @param product   pointer to the product
 * @return  index in the array, or -1 if not found
 */
int find_product_idx_by_creation(Sys *sys, Product *product){
    int i;

    for (i = 0; i < sys->unique_products; i++) {
        if (sys->products_by_creation[i] == product)
            return i;
    }
    return -1;
}

/** Find a product pointer by its EAN
 * @param sys   global system state
 * @param ean   EAN code
 * @return  pointer to the product, or NULL
 */
Product* find_product(Sys *sys, char ean[EANMAX]){
    int idx = find_product_idx_by_ean(sys, ean);

    if (idx == -1)
        return NULL;
    return sys->products_by_ean[idx];
}

/** Create and add a new product to the system, keeping arrays sorted
 * @param sys               global system state
 * @param ean               EAN code
 * @param iva_class         IVA class
 * @param price_in_cents    price in cents
 * @param quantity          initial stock
 * @param description       product description
 */
void add_product(Sys *sys, char ean[], char iva_class, 
    long price_in_cents, int quantity, char description[]){

    Product product, *p_product = NULL;
    int i, found_idx = -1;

    strcpy(product.ean, ean);
    product.iva = iva_class;
    product.price_in_cents = price_in_cents;
    product.stock = quantity;
    product.amount_sold = 0;
    strcpy(product.description, description);
    p_product = (Product *) safe_malloc(sys, sizeof(Product));
    *p_product = product;
    sys->products_by_creation[sys->unique_products] = p_product;
    /* find sorted insertion index */
    for(i = 0; i < sys->unique_products; i++){
        if ((is_ean_smaller(ean, sys->products_by_ean[i]->ean))){
            found_idx = i;
            break;
        }
    }
    if (found_idx == -1) 
        found_idx = sys->unique_products;
    /* shift array to maintain alphabetical EAN order */
    for (i = sys->unique_products - 1; i >= found_idx; i--) {
        sys->products_by_ean[i + 1] = sys->products_by_ean[i];
    }
    /* Insert the new_product on the right idx */
    sys->products_by_ean[found_idx] = p_product;
}

/** Print product details to standard output
 * Format "(Ean) (Iva) (Price) (Amount Sold) (Stock) (Description)"
 * @param p     pointer to the product
 */
void print_product(Product *p){
    char formatted_money[MONEYMAX];
    format_money(p->price_in_cents, formatted_money);
    printf("%s %c %s %d %d %s\n",
    p->ean, p->iva, formatted_money, p->amount_sold, p->stock, p->description);
}

/** Match an EAN against a wildcard pattern (?, *)
 * @param wildcard  pattern string
 * @param ean       EAN code to match
 * @return  1 if match, 0 otherwise
 */
int wildcard_match(char *wildcard, char *ean){
    int i = 0, j = 0, i_save = - 1, j_save = - 1;
    int lenean = strlen(ean);

    /* traverse the EAN string */
    while (j < lenean) {
        /* direct match or single character wildcard '?' */
        if (wildcard[i] == ean[j] || wildcard[i] == '?'){
            i++;
            j++;
        }
        /* star found: save state and assume zero characters matched */
        else if (wildcard[i] == '*'){
            i_save = i;
            j_save = j;
            i++;
        }
        /* mismatch: if a star was previously seen, 
        backtrack and try matching more */
        else if (i_save != -1){
            i = i_save + 1;
            j_save++;
            j = j_save;
        }
        /* absolute mismatch with no star to recover */
        else{
            return 0;   
        }
    }
    /* consume any remaining stars in the pattern */
    for (; wildcard[i] == '*'; i++);
    /* valid match only if both strings are exhausted */
    if (wildcard[i] == '\0') 
        return 1;
    return 0;
}

/** Print products that match a wildcard pattern
 * @param sys       global system state
 * @param tok_wild  wildcard token
 */
void print_products_wildcards(Sys *sys, char *tok_wild){
    Product *p;
    int i, product_found = 0;

    if (tok_wild == NULL)
        tok_wild = "*";
    for (i = 0; i < sys->unique_products; i++){  
        p = sys->products_by_creation[i];
        if (p->stock > 0 && wildcard_match((tok_wild), p->ean)){
            print_product(p);
            product_found = 1;
        }
    } 
    if (!product_found)
        printf("%s%s\n", tok_wild, ENOPRODUCT);
}

/** Handle the command to add or update a product
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_add_product(Sys *sys, char buffer[BUFMAX]){
    char ean[BUFMAX], decription[BUFMAX], iva;
    double price;
    int quantity;
    long price_in_cents;
    Product *product;  

    sscanf(buffer + 1, " %s %c %lf %d %[^\n]", 
        ean, &iva, &price, &quantity, decription);
    if (!is_valid_product_data(sys, ean, iva, price, quantity, decription))
        return;
    product = find_product(sys, ean);
    price_in_cents = euros_to_cents(price);
    /* add new or update existing */
    if (product == NULL){
        add_product(sys, ean, iva, price_in_cents, quantity, decription);
        printf("%d\n", 
            (sys->products_by_creation[sys->unique_products]->stock));
        sys->unique_products += 1;
    }
    else{
        update_product(product, iva, price_in_cents, quantity, decription);
        printf("%d\n", (product->stock));
    }
}

/** Handle the command to list products with optional wildcards
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_list_products(Sys *sys, char buffer[BUFMAX]){
    /* get first wildcard */
    char *tok_wild = strtok(buffer + 1, " \t\n"); 
    
    /* no args: list all (default) */
    if (tok_wild == NULL){
        print_products_wildcards(sys, NULL);
        return;
    }
    /* process all wildcards in the same line */
    while (tok_wild != NULL) {
        print_products_wildcards(sys, tok_wild);
        /* next token */
        tok_wild = strtok(NULL, " \t\n");
    }
}

#include <stdlib.h>
#include <string.h>
ListaLivro *insere(ListaLivro *lst, int preco, char titulo[]){
    ListaLivro *previous = NULL;
    ListaLivro *head_lista = lst;
    ListaLivro *aux = (*ListaLivro)malloc(sizeof(ListaLivro));
    aux->preco = preco;
    aux->titulo = strdup(titulo);
    aux->proximo = NULL;
    if (head_lista == NULL){
        head_lista = aux;
        return head_lista;
    }
    while (lst != NULL){
        if (preco < lst->preco){
            if (previous == NULL){
                head_lista = aux;
                aux->proximo = lst;
                return head_lista;
            }
            previous->proximo = aux;
            aux->proximo = lst;
            return head_lista;
        }
        previous=lst;
        lst = lst->proximo;
    }
    previous->proximo = aux;
    return head_lista;
}