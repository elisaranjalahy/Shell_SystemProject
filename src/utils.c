#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>


//////
//          Misc
//////

unsigned int stou(char* str){
    // string to unsigned int
    int res = 0;

    for (; str && (*str >= 48 && *str <= 57); str++) {
        res = res * 10 + (*str - 48);
    }
    return res;
}

char* utos(unsigned int n){
    int decimals = (n == 0);
    for(int m=n; m > 0; m /= 10){decimals++;}
    // unsigned int to string
    char* res = malloc(sizeof(char)*(decimals+1));
    if (res == NULL){exit(1);}

    int i = decimals;
    for(; n > 0 || i > 0; n /= 10){
        res[--i] = (char)((n % 10)+48);
    }
    res[decimals] = '\0';
    return res;
}

int my_strlen(const char* str){
    // Careful, this should only be used on properly formatted string.
    int j = 0;
    for(int i=0;;i++){
        if (str[i] == '\0'){return i-j;}
        if (str[i] == '\001'){j++;do {i++;j++;} while(str[i] != '\002' && str[i] != '\0');}
    }
}

char* get_home_location(char* jsh_loc){
    char* home_loc = calloc(MAX_STRING_LENGTH, sizeof(char));
    if (home_loc == NULL){exit(0);}

    int j = 0;
    for (int i = 1; jsh_loc[i] != '\0'; ++i) {
        if (jsh_loc[i] == '/') {
            if (j > 0) {
                home_loc[j++] = '/';
            }
            home_loc[j++] = '.';
            home_loc[j++] = '.';
        }
    }
    return home_loc;
}

//////
//          Jobs
//////

job_list* new_job_list(){
    job_list* list = malloc(sizeof(job_list));
    list->length = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// /home/test/jsh
// ../..