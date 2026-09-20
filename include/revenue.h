/**
 * Revenue and tax reporting operations
 * @file: revenue.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef REVENUE_H
#define REVENUE_H

#include "structs.h"

/** Handle the command to show revenue
 * @param sys       global system state
 * @param buffer    input line
 */
void cmd_show_revenue(Sys *sys, char buffer[BUFMAX]);

#endif