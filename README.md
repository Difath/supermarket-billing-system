# Supermarket Billing System

A command-line supermarket billing system written in C. Manages products (with EAN barcode validation), shopping baskets, invoices, client tracking via a Binary Search Tree (BST), and revenue reporting — all with configurable VAT (IVA) rates.

## Table of Contents
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Quick Start](#quick-start)
  - [Prerequisites](#prerequisites)
  - [Build & Run](#build--run)
- [Repository Structure](#repository-structure)
- [How It Works & Usage](#how-it-works--usage)
  - [Commands Overview](#commands-overview)
  - [Product Management](#product-management)
  - [Basket Operations](#basket-operations)
  - [Checkout & Invoicing](#checkout--invoicing)
  - [Revenue & Statistics](#revenue--statistics)
  - [Delete Operations](#delete-operations)
- [Data Structures](#data-structures)
- [Configuration](#configuration)
- [System Limits](#system-limits)
- [Error Messages](#error-messages)

## Features
- **Product Management**: Add/update products with EAN-8/EAN-13 barcode validation (checksum), price, stock, and description.
- **Wildcard Search**: Query products using `?` (single char) and `*` (zero or more chars) patterns on EAN codes.
- **Shopping Baskets**: Sorted linked list basket with stock reservation, add/remove items, and per-item VAT calculations.
- **Client Tracking**: Binary Search Tree (BST) of clients ordered alphabetically, each with a linked list of invoices.
- **Invoice Generation**: Checkout baskets to create invoices tied to clients (with NIF validation).
- **Revenue Reporting**: Track total revenue and per-product sales across all invoices.
- **Configurable VAT**: Load custom IVA rates from an external file, or use defaults (0%, 6%, 13%, 23%).
- **UTF-8 Support**: Product descriptions support Portuguese accented characters (Á, À, Â, Ã, Ç, É, Ê, Í, Ó, Ô, Õ, Ú).

## Tech Stack
- **Language**: C
- **Build**: GCC

## Quick Start

### Prerequisites
- **GCC** (or any C compiler) installed on your system.

### Build & Run
1. Clone the repository:
```bash
git clone https://github.com/Difath/supermarket-billing-system.git
cd supermarket-billing-system
```

2. Compile the project:
```bash
gcc -o proj main.c config.c helpers.c products.c basket.c invoice.c remove.c revenue.c -Wall -Wextra
```

3. Run the program (with default IVA rates):
```bash
./proj
```

Or with a custom IVA configuration file:
```bash
./proj ivaZ.txt
```

The program reads commands from stdin. Type a command letter followed by its arguments and press Enter.

## Repository Structure
```bash
/.
├── main.c               # Entry point and command dispatcher
├── structs.h            # Data structures, constants, and error strings
├── config.c             # System initialization and IVA rate loading
├── config.h             # Config function declarations
├── helpers.c            # Utility functions (currency, memory, strings)
├── helpers.h            # Helper function declarations
├── products.c           # Product management (add, list, EAN validation, wildcards)
├── products.h           # Product function declarations
├── basket.c             # Basket operations (add/remove items, tax calculations)
├── basket.h             # Basket function declarations
├── invoice.c            # Client BST, invoice creation, checkout, list invoices
├── invoice.h            # Invoice function declarations
├── remove.c             # Delete invoices and reduce/remove product stock
├── remove.h             # Remove function declarations
├── revenue.c            # Revenue reporting (total and per product)
├── revenue.h            # Revenue function declarations
├── ivaZ.txt             # Sample custom VAT rate configuration
├── .gitignore           # Git ignore rules
├── LICENSE              # MIT License
└── README.md            # Project documentation
```

## How It Works & Usage

The program runs in a loop reading single-character commands from stdin. Each command performs an operation on the system.

### Commands Overview

| Command | Name | Description |
|---|---|---|
| `p` | Add/Update Product | Register a new product or update an existing one |
| `l` | List Products | List all products or search with wildcard patterns |
| `a` | Basket Operations | Add/remove items from basket or list basket contents |
| `f` | Checkout | Finalize the basket and generate an invoice |
| `c` | List Invoices | List all invoices or invoices for a specific client |
| `r` | Revenue | Show system stats or per-product stock/sales |
| `d` | Delete | Delete an invoice or reduce/remove product stock |
| `q` | Quit | Exit the program (frees all memory) |

---

### Product Management

#### `p` — Add or Update Product

Registers a new product or updates an existing one. Products are identified by their EAN barcode.

**Input:**
```text
p <ean> <iva_class> <price> <quantity> <description>
```

| Parameter | Type | Description |
|---|---|---|
| `ean` | string | EAN-8 or EAN-13 barcode (validated with checksum) |
| `iva_class` | char | IVA category letter (`A`–`Z`, must be configured) |
| `price` | float | Price in euros (e.g., `1.50`) |
| `quantity` | int | Stock quantity to add (≥ 0) |
| `description` | string | Product name (max 50 chars, must start with uppercase) |

**Example:**
```text
p 5601234567890 C 1.25 50 Leite meio gordo
```

**Output:**
```text
50
```
Prints the current total stock of the product.

**Behavior:**
- **New product**: Creates and inserts into both creation-order and EAN-sorted arrays.
- **Existing product**: Updates IVA, price, description, and adds the quantity to existing stock.
- If the product is currently in the active basket and the new price differs, the update is rejected (`product in use`).

---

#### `l` — List Products

Lists products with stock > 0. Supports wildcard patterns on EAN codes.

**Input:**
```text
l                          # List all products (creation order)
l <pattern1> [pattern2 ...] # Search by EAN pattern
```

Wildcard characters:
- `?` — Matches any single character
- `*` — Matches zero or more characters

**Example:**
```text
l 560*
```

**Output:**
```text
5601234567890 C 1.25 0 50 Leite meio gordo
```
Format: `<ean> <iva_class> <price> <amount_sold> <stock> <description>`

---

### Basket Operations

#### `a` — Basket Operations

Manages the active shopping basket. This command has three modes:

**List basket contents:**
```text
a
```

**Add 1 unit to basket:**
```text
a <ean>
```

**Add/remove N units:**
```text
a <quantity> <ean>
```
Use a negative quantity to remove items from the basket.

| Parameter | Type | Description |
|---|---|---|
| `ean` | string | Product EAN barcode |
| `quantity` | int | Units to add (positive) or remove (negative) |

**Example:**
```text
a 3 5601234567890
```
Adds 3 units of the product to the basket.

**Output:**
```text
C 1.25 3 4.61 Leite meio gordo
```
Format: `<iva_class> <unit_price> <quantity> <total_with_vat> <description>`

**Behavior:**
- Adding items deducts stock from the product and reserves it in the basket.
- Items are maintained in a sorted linked list (by EAN).
- VAT is calculated per item using: `(total_price × (100 + tax_rate) + 50) / 100`

---

### Checkout & Invoicing

#### `f` — Checkout

Finalizes the current basket and generates an invoice. The basket is then cleared for the next customer.

**Input:**
```text
f                       # Anonymous checkout (NIF: 999999999, Name: "Cliente final")
f <name>                # Checkout with client name
f <nif> <name>          # Checkout with NIF and client name
```

| Parameter | Type | Description |
|---|---|---|
| `nif` | string | 9-digit tax ID (cannot start with `0`) |
| `name` | string | Client name (must start with letter) |

**Special:** If name is `"error"`, the checkout is cancelled — all reserved stock is returned and the basket is cleared.

**Example:**
```text
f 123456789 Joao Silva
```

**Output:**
```text
5 18.50 1
```
Format: `<total_items> <total_amount> <invoice_id>`

**Behavior:**
- Creates an `InvoiceNode` linked to the client in the BST.
- Updates global revenue counters.
- Clears the basket for the next customer.

---

#### `c` — List Invoices

Lists invoices grouped by client (alphabetical order via BST in-order traversal).

**Input:**
```text
c                   # List all invoices
c <client_name>     # List invoices for a specific client
```

**Output:**
```text
1 18.50 Joao Silva
2 5.30 Maria Santos
```
Format: `<invoice_id> <total_amount> <client_name>`

---

### Revenue & Statistics

#### `r` — Revenue

Shows system-wide statistics or per-product stock/sales data.

**System statistics:**
```text
r
```

**Output:**
```text
15 3 42.80
A 0%
B 6%
C 13%
D 23%
```
Format line 1: `<total_items_bought> <num_invoices> <total_revenue>`
Followed by all active IVA rates.

**Per-product statistics:**
```text
r <ean>
```

**Output:**
```text
47 3 Leite meio gordo
```
Format: `<stock> <amount_sold> <description>`

---

### Delete Operations

#### `d` — Delete

Deletes an invoice or reduces/removes product stock. The command has two modes based on arguments:

**Delete an invoice:**
```text
d <invoice_id>
```

**Output:**
```text
18.50 123456789 Joao Silva
```
Format: `<total_amount> <nif> <client_name>`

**Reduce product stock:**
```text
d <ean> <quantity>
```

**Output:**
```text
20 Leite meio gordo
```
Format: `<remaining_stock> <description>`

If remaining stock reaches 0, the product is completely removed from the system.

---

## Data Structures

### Product
```c
typedef struct {
    char description[51];    // Product name (max 50 chars)
    char ean[14];            // EAN-8 or EAN-13 barcode
    long price_in_cents;     // Price stored as integer cents
    char iva;                // IVA class letter ('A'–'Z')
    int stock;               // Current inventory
    int amount_sold;         // Lifetime units sold
} Product;
```

### Basket (Sorted Linked List)
```c
typedef struct BasketNode {
    Product *product;        // Pointer to the product
    int quantity;            // Quantity in basket
    struct BasketNode *next; // Next item (sorted by EAN)
} BasketNode;
```

### Client BST
```c
typedef struct ClientNode {
    char *name;              // Client name (dynamically allocated)
    InvoiceNode *head;       // First invoice
    InvoiceNode *tail;       // Last invoice (O(1) appends)
    struct ClientNode *left; // Left child (alphabetically before)
    struct ClientNode *right;// Right child (alphabetically after)
} ClientNode;
```

### Invoice (Linked List)
```c
typedef struct InvoiceNode {
    int invoice_id;                   // Unique invoice ID
    char nif[10];                     // Client NIF
    int items_bought;                 // Total items in invoice
    long total_amount_paid_in_cents;  // Total paid (cents)
    ClientNode *owner;                // Back-pointer to client
    struct InvoiceNode *next;         // Next invoice
} InvoiceNode;
```

## Configuration

### VAT Rates

**Default rates** (when no config file is provided):

| Class | Rate |
|---|---|
| `A` | 0% |
| `B` | 6% |
| `C` | 13% |
| `D` | 23% |

**Custom rates** can be loaded from a file (e.g., `ivaZ.txt`):
```text
Z 12
Q 8
V 35
M 3
```
Each line: `<class_letter> <rate_percentage>`

Run with: `./proj ivaZ.txt`

## System Limits

| Constant | Value | Description |
|---|---|---|
| `PRODMAX` | 10,000 | Maximum unique products |
| `IVAMAX` | 26 | Maximum IVA classes (A–Z) |
| `BUFMAX` | 65,536 | Maximum input line buffer |
| `DESCRIPTIONMAX` | 51 | Maximum description length (50 + null) |
| `EANMAX` | 14 | Maximum EAN buffer size |

## Error Messages

| Error | Meaning |
|---|---|
| `Comando Inválido.` | Unrecognized command |
| `invalid ean` | EAN format or checksum is invalid |
| `invalid iva` | IVA class not configured |
| `invalid price` | Price is zero or negative |
| `invalid quantity` | Quantity is invalid |
| `invalid description` | Description is empty, too long, or doesn't start with uppercase |
| `invalid product` | Maximum product limit reached |
| `<ean>: no such product` | Product with this EAN doesn't exist |
| `no stock` | Not enough stock available |
| `product in use` | Product is in the active basket (cannot modify price or delete) |
| `invalid name` | Client name is invalid |
| `<nif>: no such nif` | NIF format is invalid |
| `<name>: no such client` | Client not found in the system |
| `<id>: no such invoice` | Invoice doesn't exist or was already deleted |
| `No memory` | Memory allocation failed |

## License
[MIT](https://choosealicense.com/licenses/mit/)
