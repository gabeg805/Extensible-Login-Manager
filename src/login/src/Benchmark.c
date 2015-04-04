#include "../hdr/Benchmark.h"

double benchmark_runtime(double start) {
    struct timespec t;
    /* clock_gettime(CLOCK_REALTIME, &t); */
    clock_gettime(CLOCK_MONOTONIC, &t);
    /* clock_gettime(CLOCK_MONOTONIC_COARSE, &t); */
    /* clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t); */
    /* clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t); */
    
    double time = (double)(t.tv_sec + (t.tv_nsec / 1e9));
    if ( start != 0 )
        return (time - start);
    else
        return time;
}
