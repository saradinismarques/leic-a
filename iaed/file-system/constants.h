/*
 * File: constants.h
 * Author: Sara Marques
 * Number: 93342
 * Description: This file contains all the constants used in the system.
 */

#ifndef CONSTANTS
#define CONSTANTS

/* Maxium size of a buffer to help read the input. */
#define MAX_BUFFER 65536

/* Slash used to divide each component of a path. */
#define SLASH "/"

/* Name of the root path. */
#define ROOT "/"

/* Delimitations to help read each command. */
#define CMD_DELIM " \t\n"
#define PATH_DELIM " \t\n"
#define VALUE_DELIM "\n"

/* Command names. */
#define HELP "help"
#define QUIT "quit"
#define SET "set"
#define PRINT "print"
#define FIND "find"
#define LIST "list"
#define SEARCH "search"
#define DELETE "delete"

/* Command descriptions. */
#define HELP_DESCRIPTION "Imprime os comandos disponíveis."
#define QUIT_DESCRIPTION "Termina o programa."
#define SET_DESCRIPTION "Adiciona ou modifica o valor a armazenar."
#define PRINT_DESCRIPTION "Imprime todos os caminhos e valores."
#define FIND_DESCRIPTION "Imprime o valor armazenado."
#define LIST_DESCRIPTION "Lista todos os componentes imediatos de um sub-caminho."
#define SEARCH_DESCRIPTION "Procura o caminho dado um valor."
#define DELETE_DESCRIPTION "Apaga um caminho e todos os subcaminhos."

/* Error Messages. */
#define NOT_FOUND "not found"
#define NO_DATA "no data"
#define NO_MEMORY "no memory"

/* Returning Values. */
#define ERROR 0
#define SUCCESS 1

#endif
