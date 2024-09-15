#include "cache.h"
#include "iterator.h"
#include "string.h"

typedef char out_buffer[0xFFFF];

typedef struct
{
    int hits;
    int misses;
    int evictions;
} cache_stats_t;

/// @brief Runs a cache simulation using the given cache and trace file.
/// @param cache the cache to simulate
/// @param file the valgrind trace file to read from
/// @param debug output buffer to write debug information to
/// @return hits, misses, and evictions on the cache
cache_stats_t simulate(cache_t cache, FILE *file, out_buffer debug)
{
    cache_stats_t stats = {0, 0, 0};
    trace_line_t line;
    cache_result_t result;
    for (line = yield_trace_line(file); line.flag != U; line = yield_trace_line(file))
    {
        if (line.flag == M || line.flag == S)
        {
            result = write_cache(cache, line.address);
            result.hit ? (stats.hits)++ : (stats.misses)++;
            if (result.eviction)
            {
                (stats.evictions)++;
            }

            if (debug != NULL)
            {
                debug += sprintf(debug, "%c %llx %s %s\n", line.flag == M ? 'M' : 'S', line.address, result.hit ? "hit" : "miss", result.eviction ? "eviction" : "");
            }
        }

        if (line.flag == M || line.flag == L)
        {
            result = read_cache(cache, line.address);
            result.hit ? (stats.hits)++ : (stats.misses)++;
            if (result.eviction)
            {
                (stats.evictions)++;
            }

            if (debug != NULL)
            {
                debug += sprintf(debug, "%c %llx %s %s\n", line.flag == M ? 'M' : 'L', line.address, result.hit ? "hit" : "miss", result.eviction ? "eviction" : "");
            }
        }
    }

    return stats;
}