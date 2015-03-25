#include "../hdr/Benchmark.h"

double benchmark_runtime(double start) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    
    double time = (double)(t.tv_sec + (t.tv_nsec / 1e9));
    if ( start != 0 )
        return (time - start);
    else
        return time;
}
