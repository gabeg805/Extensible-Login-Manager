/* *****************************************************************************
 * 
 * Name:    elmalloc.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Memory allocation and handling.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_ALLOC_H
#define ELM_ALLOC_H

/* Includes */
#include <stdlib.h>

/* Public functions  */
void * elm_calloc(void *ptr, size_t nmemb, size_t size);
void * elm_realloc(void *ptr, size_t nmemb, size_t size);
void   elm_free(void *ptr);

#endif /* ELM_ALLOC_H */
