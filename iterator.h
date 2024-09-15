#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/// @brief Valgrind trace line operation flags.
typedef enum
{
    U = -1, // undefined
    I = 0,  // instruction load
    L = 1,  // data load
    S = 2,  // data store
    M = 3   // data modify (data load followed by data store)
} v_opt_flag;

/// @brief Valgrind trace line
typedef struct
{
    v_opt_flag flag;
    u_int64_t address;
} trace_line_t;

trace_line_t yield_trace_line(FILE *file)
{
    if (feof(file))
    {
        return (trace_line_t){.flag = U, .address = 0};
    }

    /*
        Assignment note:

        "You should assume that memory accesses are aligned properly, such that a single memory
        access never crosses block boundaries. By making this assumption, you can
        ignore the request sizes in the valgrind traces."
    */
    trace_line_t line = {.flag = U, .address = 0};
    char flag;
    u_int8_t ignore;
    fscanf(file, " %c %llx,%hhu", &flag, &line.address, &ignore);
    switch (flag)
    {
    case 'I':
        line.flag = I;
        break;
    case 'L':
        line.flag = L;
        break;
    case 'S':
        line.flag = S;
        break;
    case 'M':
        line.flag = M;
        break;
    }
    return line;
}