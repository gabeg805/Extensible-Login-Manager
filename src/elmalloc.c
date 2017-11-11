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

/* Includes */
#include "elmalloc.h"
#include <stdlib.h>

/* ************************************************************************** */
/* Wrapper for calloc() that does pre and post checking for you. This will not
 * allocate memory for a pointer that is not null. If ptr is non-nll, should be
 * reference to another pointer. */
void * elm_calloc(void *ptr, size_t nmemb, size_t size)
{
    /* Call calloc directly when pointer is null */
    if (!ptr) {
        return calloc(nmemb, size);
    }

    char **cast = ptr;
    void  *new;

    /* Allocate memory */
    if (!(new=calloc(nmemb, size))) {
        return NULL;
    }

    return (*cast=new);
}

/* ************************************************************************** */
/* Wrapper for calloc() that does post checking for you. If ptr is non-null,
 * should be reference to another pointer. */
void * elm_realloc(void *ptr, size_t nmemb, size_t size)
{
    /* Call calloc directly when pointer is null */
    if (!ptr) {
        return elm_calloc(NULL, nmemb, size);
    }

    /* Reallocate memory */
    char **cast = ptr;
    void  *new;

    if (!(new=realloc(*cast, nmemb*size))) {
        free(new);
        return NULL;
    }

    return (*cast=new);
}

/* ************************************************************************** */
/* Wrapper for free(), for completeness since other *alloc functions were
 * wrapped. */
void elm_free(void *ptr)
{
    char **ref = ptr;

    free(*ref);

    *ref = NULL;
}
