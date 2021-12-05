#include <cassert>
#include <string>
#include <fstream>
#include "utils.h"
#include <cilk/cilk_api.h>

extern void print_lcs_graph(cell_t * entry, const char * output_file);
extern void output_all_lcs_seq(cell_t * entry, const char * output_file);
extern void output_all_lcs_parallel(cell_t * entry, const char * output_folder);

static struct timespec __start, __end;

#define MEASURE(__ret_ptr, __func, ...) \
    ((clock_gettime(CLOCK_MONOTONIC, &__start), \
    *(__ret_ptr) = __func(__VA_ARGS__),       \
    clock_gettime(CLOCK_MONOTONIC, & __end)),  \
    (__end.tv_sec - __start.tv_sec) + 1e-9 * (__end.tv_nsec - __start.tv_nsec))\

#define MEASURE_NO_RETURN(__func, ...) \
    ((clock_gettime(CLOCK_MONOTONIC, &__start), \
    __func(__VA_ARGS__),       \
    clock_gettime(CLOCK_MONOTONIC, & __end)),  \
    (__end.tv_sec - __start.tv_sec) + 1e-9 * (__end.tv_nsec - __start.tv_nsec)) \
    
int main(int argc, char *argv[])
{
    assert(argc >= 2);

    std::string _input = argv[1];

    std::string A, B;

    std::ifstream input_file(_input);
    input_file >> A >> B;
    input_file.close();

    const int M = A.length();
    const int N = B.length();
    const char * A_str = A.c_str();
    const char * B_str = B.c_str();

    cell_t * entry;
    int lcs_len = naive_lcs_2d(A_str, B_str, M, N);
    int64_t lcs_cnt;

    double tdiff;

    printf("------------------------------------------\n");

    printf("M: %d, N: %d, CILK has %d workers\n", M, N, __cilkrts_get_nworkers());

    printf("LCS length: %d\n", lcs_len);

    tdiff = MEASURE(&entry, lcs_anticanonical, A_str, B_str, M, N);
    printf("anticanonical: %.3f (s)\n", tdiff);

    tdiff = MEASURE_NO_RETURN(print_lcs_graph, entry, "./output/graph.txt");
    printf("print graph: %.3f (s)\n", tdiff);

    tdiff = MEASURE_NO_RETURN(output_all_lcs_seq, entry, "./output/seq.txt");
    printf("lcs traversal (seq): %.3f (s)\n", tdiff);

    tdiff = MEASURE_NO_RETURN(output_all_lcs_parallel, entry, "./output");
    printf("lcs traversal (parallel): %.3f (s)\n", tdiff);

    cell_t * table = entry + 1 - (M + 1) * (N + 1);
    for(int i = 1; i <= M; i++)
    {
        for(int j = 1; j <= N; j++)
        {
            free(table[i * (N + 1) + j].next_list);
        }
    }
    free(table);

    return 0;
}