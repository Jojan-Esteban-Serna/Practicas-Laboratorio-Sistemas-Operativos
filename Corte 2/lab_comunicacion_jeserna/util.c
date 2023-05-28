/**
 * @file util.c
 * @author your name (you@domain.com)
 * @brief Implemenation of util.h
 * @version 0.1
 * @date 2023-05-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <ctype.h>

char * to_upper(char * str){
    char *aux = str;
    while(*aux != 0){
        *aux = toupper(*aux);
        aux++;
    }

}