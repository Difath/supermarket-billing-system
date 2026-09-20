/**
 * Product operations
 * @file: products.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef PRODUCTS_H
#define PRODUCTS_H

#include "structs.h"

/** Validate EAN-13 or EAN-8 format and check digit
 * @param ean   EAN string to validate
 * @return  1 if valid, 0 otherwise
 */
int is_valid_ean(char ean[EANMAX]);

/** Find a product index in the EAN sorted array using binary search
 * @param sys   global system state
 * @param ean   EAN code to search
 * @return  index in the array, or -1 if not found
 */
int find_product_idx_by_ean(Sys *sys, char *ean);

/** Find a product index in the creation order array
 * @param sys       global system state
 * @param product   pointer to the product
 * @return  index in the array, or -1 if not found
 */
int find_product_idx_by_creation(Sys *sys, Product *product);

/** Finder a product pointer by its EAN
 * @param sys   global system state
 * @param ean   EAN code
 * @return  pointer to the product, or NULL
 */
Product* find_product(Sys *sys, char ean[EANMAX]);

/** Print product details to standard output
 * Format "(Ean) (Iva) (Price) (Amount Sold) (Stock) (Description)"
 * @param p     pointer to the product
 */
void print_product(Product *p);

/** Handle the command to add or update a product
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_add_product(Sys *sys, char buffer[BUFMAX]);

/** Handle the command to list products with optional wildcards
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_list_products(Sys *sys, char buffer[BUFMAX]);

/** Compare two EAN strings alphabetically
 * @param ean1  first EAN
 * @param ean2  second EAN
 * @return  1 if ean1 < ean2, 0 otherwise
 */
int is_ean_smaller(char ean1[], char ean2[]);

#endif