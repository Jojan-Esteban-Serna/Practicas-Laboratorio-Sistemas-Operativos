/**
 * @file
 * @brief String split library
 * @author Erwin Meza Vega <emezav@gmail.com> 
*/

#ifndef SPLIT_H_
#define SPLIT_H_

#define MAX_PARTS 255

/**
 * @brief Define la estructura de la lista de palabras obtenida como valor de retorno
 * de la funci�n split.
 */

typedef struct {
    char * parts[MAX_PARTS]; /*!<Partes obtenidas*/
    int count;/*!<cantidad partes obtenidas*/
}split_list;

/** 
 * 
 */

/**
 * @brief Divide una cadena por los delimitadores especificados
 * @param str  Cadena a dividir
 * @param delim Cadena que contine los delimitadores
 * @return Estructura de datos con las partes de la linea
 * Divide una cadena en palabras, usando los delimitadores especificados
 * o los delimitadores por defecto
 */
split_list * split(char * str, const char * delim);

#endif
