#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include "cachesim.h"

void gather_input_args(int argc, char *argv[], int *s, int *E, int *b, int *verbose, char *traceFile);
void check_argument(int condition, const char *argument);
void print_summary(int hits, int misses, int evictions);
void print_usage(char *argv[]);

int main(int argc, char *argv[])
{
    // #region GATHER_INPUT_ARGS
    int s = 0, E = 0, b = 0, verbose = 0;
    char path[100] = "";
    gather_input_args(argc, argv, &s, &E, &b, &verbose, path);

    check_argument(s == 0, "s");
    check_argument(E == 0, "E");
    check_argument(b == 0, "b");
    check_argument(path[0] == '\0', "t");
    // #endregion GATHER_INPUT_ARGS

    // #region TRACE_FILE
    FILE *trace_file = fopen(path, "r");
    if (path[0] == '\0')
    {
        fprintf(stderr, "Error opening file: %s\n", path);
        exit(1);
    }
    // #endregion TRACE_FILE

    // #region SIMULATE
    cache_t cache = create_cache(s, E, b);

    cache_stats_t stats;
    if (verbose)
    {
        out_buffer *debug = malloc(sizeof(out_buffer));
        assert(debug != NULL);

        stats = simulate(cache, trace_file, NULL);
        save_debug(*debug, sprintf("traces/output/verbose.txt", path));
    }
    else
    {
        stats = simulate(cache, trace_file, NULL);
    }

    print_summary(stats.hits, stats.misses, stats.evictions);
    // #endregion SIMULATE

    free_cache(cache);
    fclose(trace_file);
    return 0;
}

void check_argument(int condition, const char *argument)
{
    if (condition)
    {
        fprintf(stderr, "Missing required argument: %s\n", argument);
        exit(1);
    }
}

void gather_input_args(int argc, char *argv[], int *s, int *E, int *b, int *verbose, char *traceFile)
{
    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_usage(argv);
            break;
        case 'v':
            *verbose = 1;
            break;
        case 's':
            *s = atoi(optarg);
            break;
        case 'E':
            *E = atoi(optarg);
            break;
        case 'b':
            *b = atoi(optarg);
            break;
        case 't':
            strcpy(traceFile, optarg);
            break;
        default:
            print_usage(argv);
            break;
        }
    }
}

void print_usage(char *argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

void print_summary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
}
