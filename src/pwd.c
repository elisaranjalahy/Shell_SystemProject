#include "lib.h"


int pwd(char* cur){
    ssize_t nb = write(1,cur,strlen(cur));
    write (1,"\n",1);
    if (nb != strlen(cur)) return 1;
    return 0;
}