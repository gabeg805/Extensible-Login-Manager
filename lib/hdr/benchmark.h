/* *****************************************************************************
 * 
 * Name:    benchmark.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "benchmark.h"
 * 
 * Description: Time the progress of program execution.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_BENCHMARK_H
#define ELYSIA_BENCHMARK_H

/* Includes */
#include <stdbool.h>

/* Defines */
extern bool BENCHTIME;

/* Public functions */
double benchmark_runtime(double start);

#endif /* ELYSIA_BENCHMARK_H */
