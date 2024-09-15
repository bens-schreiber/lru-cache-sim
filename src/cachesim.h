#include "cache.h"
#include "iterator.h"
#include "string.h"

typedef char out_buffer[0xFFFFF];

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
    for (line = yield_trace_line(file); line.flag != U; line = yield_trace_line(file))
    {

        if (line.flag == S || line.flag == L)
        {
            cache_result_t result = (line.flag == S) ? write_cache(cache, line.address) : read_cache(cache, line.address);
            result.hit ? (stats.hits)++ : (stats.misses)++;
            stats.evictions += result.eviction;

            if (debug != NULL)
            {
                debug += sprintf(debug, "%c %llx,%hhu %s %s\n", line.flag == S ? 'S' : 'L', line.address, line.size, result.hit ? "hit" : "miss", result.eviction ? "eviction" : "");
            }
        }

        else if (line.flag == M)
        {
            cache_result_t result1 = read_cache(cache, line.address);
            cache_result_t result2 = write_cache(cache, line.address);

            stats.hits += result1.hit + result2.hit;
            stats.misses += !result1.hit + !result2.hit;
            stats.evictions += result1.eviction + result2.eviction;

            if (debug != NULL)
            {
                debug += sprintf(debug, "M %llx,%hhu %s %s %s %s\n", line.address, line.size, result1.hit ? "hit" : "miss", result2.hit ? "hit" : "miss", result1.eviction ? "eviction" : "", result2.eviction ? "eviction" : "");
            }
        }
    }

    if (debug != NULL)
    {
        debug += sprintf(debug, "hits:%d misses:%d evictions:%d\n", stats.hits, stats.misses, stats.evictions);
    }

    return stats;
}

void save_debug(out_buffer debug, const char *path)
{
    FILE *debug_file = fopen(path, "w");
    if (debug_file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", path);
        exit(1);
    }
    fprintf(debug_file, "%s", debug);
    fclose(debug_file);
}