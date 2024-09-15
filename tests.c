#include <stdio.h>
#include "cachesim.h"

// #region ITERATOR_TESTS
void _test_iterator(const char *path)
{
    // Arrange
    FILE *file = fopen(path, "r");
    assert(file != NULL);

    // Act
    trace_line_t line;
    while (feof(file) == 0)
    {
        line = yield_trace_line(file);
    }

    assert(line.flag == U);

    // Assert
    fclose(file);
}

#define test_iterator_inputs {"traces/trace01.dat", "traces/trace02.dat", "traces/trace03.dat", "traces/trace04.dat"}
void test_iterator()
{
    const char *in[4] = test_iterator_inputs;
    for (int i = 0; i < 4; i++)
    {
        _test_iterator(in[i]);
    }
}
// #endregion ITERATOR_TESTS

// #region CACHE_TESTS
void test_create_cache()
{
    // Arrange
    int s = 4, E = 1, b = 4;

    // Act
    cache_t cache = create_cache(s, E, b);

    // Assert
    assert(cache.s == s);
    assert(cache.E == E);
    assert(cache.b == b);
    assert(cache.data != NULL);
    free_cache(cache);
}

void test_cache_miss()
{
    // Arrange
    int s = 4, E = 1, b = 4;
    cache_t cache = create_cache(s, E, b);
    u_int64_t address = 0x12345678;

    // Act
    cache_result_t result = write_cache(cache, address);

    // Assert
    assert(result.hit == 0);
    free_cache(cache);
}

void test_cache_hit()
{
    // Arrange
    int s = 4, E = 1, b = 4;
    cache_t cache = create_cache(s, E, b);
    u_int64_t address = 0x12345678;
    write_cache(cache, address);

    // Act
    cache_result_t result = read_cache(cache, address);

    // Assert
    assert(result.hit == 1);
    free_cache(cache);
}

void test_cache_eviction()
{
    // Arrange
    int s = 4, E = 2, b = 4;
    cache_t cache = create_cache(s, E, b);
    u_int64_t address1 = 0x12345678;
    u_int64_t address2 = 0x12345678 + (1 << b); // Ensure same set index as address1
    u_int64_t address3 = 0x12345678 + (2 << b); // Ensure same set index as address1 and address2
    write_cache(cache, address1);
    write_cache(cache, address2);

    // Act
    cache_result_t result = write_cache(cache, address3);

    // Assert
    assert(result.eviction == 1);
    free_cache(cache);
}
// #endregion CACHE_TESTS

// #region INTERGRATION_TESTS
void test_trace01()
{
    // Arrange
    int s = 1, E = 1, b = 1;
    const char *path = "traces/trace01.dat";
    FILE *file = fopen(path, "r");
    assert(file != NULL);

    int hit_count = 0, miss_count = 0, eviction_count = 0;
    cache_t cache = create_cache(s, E, b);
    cache_result_t result;

    // Act
    out_buffer debug;
    cache_stats_t stats = simulate(cache, file, debug);

    // Log
    FILE *debug_file = fopen("traces/output/trace01out.debug.txt", "w");
    fprintf(debug_file, "%s", debug);
    fclose(debug_file);

    // Assert
    assert(stats.hits == 9);
    assert(stats.misses == 8);
    assert(stats.evictions == 6);
}
// #endregion INTERGRATION_TESTS

int main(int argc, char const *argv[])
{
    // test_iterator();
    // test_create_cache();
    // test_cache_miss();
    // test_cache_hit();
    // test_cache_eviction();

    test_trace01();

    return 0;
}
