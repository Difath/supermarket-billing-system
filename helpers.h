/**
 * Utility and helper functions
 * @file: helpers.h
 * @author: ist1117887 (Diogo Monteiro)
 */
#ifndef HELPERS_H
#define HELPERS_H

#include "structs.h"

/** Convert double euros to long cents with rounding
 * @param euros     value in euros
 * @return          equivalent value in cents
 */
long euros_to_cents(double euros);

/** Extract the euro part from total cents
 * @param cents     total value in cents
 * @return          euro part (cents / 100)
 */
long only_euros(long cents);

/** Extract the remaining cent part from total cents
 * @param cents     total value in cents
 * @return          cent part (cents % 100)
 */
int only_cents(long cents);

/** Format a cent value into a string 
 * Format (Euros).(Cents) Ex: 12.03
 * @param total_cents      value to format
 * @param formatted_money  target buffer for the string
 * @return                 pointer to the formatted string
 */
char *format_money(long total_cents, char *formatted_money);

/** Allocate memory with error handling and safe exit
 * @param sys       global system state
 * @param size      number of bytes to allocate
 * @return          pointer to allocated memory
 */
void *safe_malloc(Sys *sys, size_t size);

/** Reallocate memory with error handling and safe exit
 * @param sys       global system state
 * @param ptr       original pointer
 * @param size      new size in bytes
 * @return          pointer to reallocated memory
 */
void *safe_realloc(Sys *sys, void *ptr, size_t size);

/** Remove a character at a specific position in a string
 * @param str       target string
 * @param pos       index of the character to remove
 */
void remove_char_at(char *str, int pos);

/** Remove surrounding double quotes from a string
 * @param str       target string
 */
void remove_quotes(char *str);

#endif