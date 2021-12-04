#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "utils.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

inline void print_node(cell_t * node)
{
    if (node == NULL) return;

    printf("(%d,%d)", node->i, node->j);

    if (node->match == '\0')
        printf("[%s]", "\\0");
    else printf("[%c]", node->match);

    if (node->next_num > 0)
        printf(": ");  

    for(int i = 0; i < node->next_num; i++)
        printf("(%d,%d) ", node->next_list[i]->i, node->next_list[i]->j); 

    printf("\n");  
}

void print_lcs_graph(cell_t * entry, const char * output_file)
{

}

void output_all_lcs_seq(cell_t * entry, const char * output_file)
{

}

void output_all_lcs_parallel(cell_t * entry, const char * output_folder)
{

}