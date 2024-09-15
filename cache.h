#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#define ADDRESS_LENGTH 64 // 64-bit memory addressing

#define cache_size(s, E, b) (s * E * b)

typedef struct cache_block_t
{
    u_int8_t valid; // 1 if the block is valid, 0 otherwise
    u_int64_t tag;
    clock_t timestamp;
} cache_block_t;

typedef struct
{
    int S; // number of sets
    int s; // set index bits

    int E; // number of lines per set

    int B; // block size
    int b; // block offset bits

    cache_block_t *data;
} cache_t;

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

typedef struct
{
    u_int8_t hit;
    u_int8_t eviction;
} cache_result_t;

cache_result_t read_cache(cache_t cache, u_int64_t address)
{
    /*
        address = [tag | set index | block offset]
    */
    u_int64_t set_index = (address >> cache.b) & ((1 << cache.s) - 1);
    u_int64_t tag = address >> (cache.s + cache.b);

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

cache_result_t write_cache(cache_t cache, u_int64_t address)
{
    /*
        address = [tag | set index | block offset]
    */
    u_int64_t set_index = (address >> cache.b) & ((1 << cache.s) - 1);
    u_int64_t tag = address >> (cache.s + cache.b);

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