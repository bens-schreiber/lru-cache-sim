#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef CACHE_H
#define CACHE_H

#define ADDRESS_LENGTH 64 // 64-bit memory addressing

#define cache_size(s, E, b) (s * E * b)

/// @brief A block of memory in the cache.
typedef struct cache_block_t
{
    int8_t valid; // 1 if the block is valid, 0 otherwise
    int64_t tag;
    clock_t timestamp; // LRU timestamp
} cache_block_t;

/// @brief A cache with a set-associative mapping given by the s E b dimensions
typedef struct
{
    int S; // number of sets
    int s; // set index bits

    int E; // number of lines per set

    int B; // block size
    int b; // block offset bits

    cache_block_t *data;
} cache_t;

/// @brief Creates a cache with the given s, E, and b dimensions.
cache_t create_cache(int s, int E, int b)
{

    int S = pow(2, s);
    int B = pow(2, b);

    cache_block_t *data = (cache_block_t *)malloc(cache_size(S, E, B) * sizeof(cache_block_t));
    assert(data != NULL);

    // set all to invalid
    cache_block_t emptyBlock = {.valid = 0, .tag = 0, .timestamp = 0};
    for (int i = 0; i < cache_size(S, E, B); i++)
    {
        data[i] = emptyBlock;
    }

    return (cache_t){
        .S = S,
        .s = s,
        .E = E,
        .B = B,
        .b = b,
        .data = data};
}

void free_cache(cache_t cache)
{
    free(cache.data);
}

/// @brief The result of a cache read or write.
typedef struct
{
    int8_t hit;      // 1 if the operation was a hit, 0 otherwise
    int8_t eviction; // 1 if the operation caused an eviction, 0 otherwise
} cache_result_t;

/// NOTE: Since no writing is actually done in this sim, the write_cache function is identical to the read_cache function.
cache_result_t _read_write_cache(cache_t cache, int64_t address)
{
    /*
        address = [tag | set index | block offset]
    */
    int64_t set_index = (address >> cache.b) & ((1 << cache.s) - 1);
    int64_t tag = address >> (cache.s + cache.b);

    // check if tag is in set
    cache_block_t *set = cache.data + (set_index * cache.E);
    for (int i = 0; i < cache.E; i++)
    {

        // hit
        if (set[i].valid && set[i].tag == tag)
        {
            set[i].timestamp = clock();
            return (cache_result_t){.hit = 1, .eviction = 0};
        }

        // miss, empty block
        if (!set[i].valid)
        {
            set[i].valid = 1;
            set[i].tag = tag;
            set[i].timestamp = clock();
            return (cache_result_t){.hit = 0, .eviction = 0};
        }
    }

    // evict LRU
    int lru_index = 0;
    clock_t lru = set[0].timestamp;
    for (int j = 1; j < cache.E; j++)
    {
        if (set[j].timestamp < lru)
        {
            lru = set[j].timestamp;
            lru_index = j;
        }
    }

    set[lru_index].tag = tag;
    set[lru_index].timestamp = clock();
    return (cache_result_t){.hit = 0, .eviction = 1};
}

cache_result_t read_cache(cache_t cache, int64_t address)
{
    return _read_write_cache(cache, address);
}

cache_result_t write_cache(cache_t cache, int64_t address)
{
    return _read_write_cache(cache, address);
}

#endif // CACHE_H