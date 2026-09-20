/**
 * Utility and helper functions (Implementation)
 * @file: helpers.c
 * @author: ist1117887 (Diogo Monteiro)
 */
#include "structs.h"
#include "helpers.h"
#include "config.h"

/** Convert double euros to long cents with rounding
 * @param euros     value in euros
 * @return          equivalent value in cents
 */
long euros_to_cents(double euros) {
    /* add 0.5 to ensure correct rounding when casting to long */
    return (long) (euros * 100.0 + 0.5);
}

/** Extract the euro part from total cents
 * @param cents     total value in cents
 * @return          euro part
 */
long only_euros(long cents) {
    return (long) (cents / 100);
}

/** Extract the remaining cent part from total cents
 * @param cents     total value in cents
 * @return          cent part
 */
int only_cents(long cents) {
    return (int) (cents % 100);
}

/** Format a cent value into a string 
 * Format (Euros).(Cents) Ex: 12.03
 * @param total_cents      value to format
 * @param formatted_money  target buffer for the string
 * @return                 pointer to the formatted string
 */
char *format_money(long total_cents, char *formatted_money){
    long euros_only = only_euros(total_cents);
    int cents_only = only_cents(total_cents);

    sprintf(formatted_money, "%ld.%02d", euros_only, cents_only);
    return formatted_money;
}

/** Allocate memory with error handling and safe exit
 * @param sys       global system state
 * @param size      number of bytes to allocate
 * @return          pointer to allocated memory
 */
void *safe_malloc(Sys *sys, size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL && size != 0) {
        puts(EMEMORY);
        safe_exit(sys); 
    }
    return ptr;
}

/** Reallocate memory with error handling
 * @param sys       global system state
 * @param ptr       original pointer
 * @param size      new size
 * @return          new pointer
 */
void *safe_realloc(Sys *sys, void *ptr, size_t size){
    void *new_ptr = realloc(ptr, size);

    if (new_ptr == NULL && size != 0) {
        puts(EMEMORY);
        safe_exit(sys);
    }
    return new_ptr;
}

/** Remove a character at a specific position
 * @param str       target string
 * @param pos       index to remove
 */
void remove_char_at(char *str, int pos) {
    int i;
    
    /* shift all characters to the left */
    for (i = pos; str[i] != '\0'; i++) {
        str[i] = str[i + 1];
    }
}

/** Remove surrounding double quotes
 * @param str       target string
 */
void remove_quotes(char *str) {
    int len = strlen(str);

    if (len >= 2 && str[0] == '"' && str[len-1] == '"'){
        str[len - 1] = '\0';
        remove_char_at(str, 0);
    }
}