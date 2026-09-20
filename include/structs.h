/**
 * Core data structures and constants for the system
 * @file: structs.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BASKET_CHECKOUT 0       /**< checkout mode for the basket */
#define BASKET_REVERT   1       /**< revert mode to cancel the basket */
#define BASKET_FREE     2       /**< free mode to destroy the basket */

#define DEFAULTRATES    {0, 6, 13, 23}              /**< default IVA rates */
#define DEFAULTCLASSES  {'A', 'B', 'C', 'D'}        /**< default IVA classes */

#define BUFMAX          65536       /**< max length of buffer */
#define PRODMAX         10000       /**< max number of unique products */
#define DESCRIPTIONMAX  51          /**< max length of product description */
#define IVAMAX          26          /**< max number of IVA classes */
#define MONEYMAX        24          /**< max length of formatted money string */
#define EANMAX          14          /**< max length of EAN string */
#define LENEAN13        13          /**< length of EAN-13 */
#define LENEAN8         8           /**< length of EAN-8 */

#define EINVALID        "Comando Inválido."     /**< invalid command format */
#define EEAN            "invalid ean"           /**< invalid EAN */
#define EIVA            "invalid iva"           /**< invalid IVA */
#define EPRICE          "invalid price"         /**< invalid price */
#define EQUANTITY       "invalid quantity"      /**< invalid quantity */
#define EDESCRIPTION    "invalid description"   /**< invalid description */
#define EPRODUCT        "invalid product"       /**< invalid product */
#define EMEMORY         "No memory"             /**< memory exhausted */
#define ENOPRODUCT      ": no such product"     /**< non existing product */
#define ESTOCK          "no stock"              /**< insufficient stock */
#define EPRODINUSE      "product in use"        /**< product is in use */
#define ENONIF          ": no such nif"         /**< invalid NIF */
#define ENAME           "invalid name"          /**< invalid name */
#define ENOCLIENT       ": no such client"      /**< non existing client */
#define ENOINVOICE      ": no such invoice"     /**< non existing invoice */

typedef struct ClientNode ClientNode;

/** product information */
typedef struct {
    char description[DESCRIPTIONMAX];   /**< product description */
    char ean[EANMAX];                   /**< EAN code */
    long price_in_cents;                /**< price in cents */
    char iva;                           /**< IVA class */
    int stock;                          /**< current stock quantity */
    int amount_sold;                    /**< total amount sold */
} Product;

/** node for a linked list of basket items */
typedef struct BasketNode{    
    Product *product;                   /**< pointer to product in system */
    int quantity;                       /**< quantity in the basket */
    struct BasketNode *next;            /**< pointer to next basket node */
} BasketNode;

/** shopping basket state */
typedef struct {
    int unique_items;                   /**< number of unique products */
    int total_items;                    /**< total quantity of items */
    BasketNode *head;                   /**< head of basket items list */
} Basket;

/** node for a linked list of invoices */
typedef struct InvoiceNode{
    int invoice_id;                     /**< unique invoice identifier */
    int nif;                            /**< client NIF */
    int items_bought;                   /**< total items in invoice */
    long total_amount_paid_in_cents;    /**< total paid amount */
    ClientNode* owner;                  /**< pointer to invoice owner */
    struct InvoiceNode* next;           /**< pointer to next invoice */
} InvoiceNode;

/** node for a binary search tree of clients, 
 * containing a linked list of invoices */
typedef struct ClientNode {
    char *name;                         /**< client name */
    InvoiceNode* head_invoices;         /**< head of client's invoices list */
    InvoiceNode* tail_invoices;         /**< tail of client's invoices list */
    struct ClientNode* left;            /**< left child in BST */
    struct ClientNode* right;           /**< right child in BST */
} ClientNode;

/** all state information stored by the system */
typedef struct {
    Product *products_by_creation[PRODMAX]; /**< products ordered by creation */
    Product *products_by_ean[PRODMAX];  /**< products ordered by EAN */
    int unique_products;                /**< number of unique products */
    
    Basket current_basket;              /**< current active basket */
    ClientNode *clients_root;           /**< root of clients BST */
    InvoiceNode **all_invoices;         /**< dynamic array of invoice pointers*/
    int n_invoices;                     /**< total number of invoices */
    
    int iva_table[IVAMAX];              /**< table of IVA rates */
    int total_items_bought;             /**< global count of items bought */
    long total_invoiced_amount_in_cents;/**< global revenue in cents */
} Sys;

#endif