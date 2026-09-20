/**
 * Product and invoice removal operations
 * @file: remove.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef REMOVE_H
#define REMOVE_H

#include "structs.h"

/** Handle the delete command (invoices or products)
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_delete(Sys *sys, char *buffer);

#endif