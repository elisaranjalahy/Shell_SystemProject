#include "utils.h"

/**
 * Changes the current directory if query is valid.
 * @param cur the current path
 * @param last the previous path
 * @param query where the user wants to go
 * @return 0 on success, else 1
 */
int my_cd(char* cur, const char* directory);