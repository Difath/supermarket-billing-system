/**
 * System configuration and initialization operations
 * @file: config.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "structs.h"

/** Safely free all allocated memory and exit the program
 * @param sys   global system state
 */
void safe_exit(Sys *sys);

/** Initialize the global system state including products, clients, and IVA
 * @param sys   global system state
 * @param argc  command line argument count
 * @param argv  command line argument vector
 */
void initialize_sys(Sys *sys, int argc, char *argv[]);

#endif