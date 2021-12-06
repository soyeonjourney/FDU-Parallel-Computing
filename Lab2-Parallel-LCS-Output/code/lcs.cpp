#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "utils.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>


/*
inline void print_node(cell_t* node) {
    if (node == NULL) return;

    printf("(%d,%d)", node->i, node->j);

    if (node->match == '\0')
        printf("[%s]", "\\0");
    else printf("[%c]", node->match);

    if (node->next_num > 0)
        printf(": ");

    for (int i = 0; i < node->next_num; i++)
        printf("(%d,%d) ", node->next_list[i]->i, node->next_list[i]->j);

    printf("\n");
}
*/

inline void fprint_node(FILE* fp, cell_t* node) {
    if (node == NULL) return;

    fprintf(fp, "(%d,%d)", node->i, node->j);

    if (node->match == '\0')
        fprintf(fp, "[%s]", "\\0");
    else fprintf(fp, "[%c]", node->match);

    if (node->next_num > 0)
        fprintf(fp, ": ");

    for (int i = 0; i < node->next_num; i++)
        fprintf(fp, "(%d,%d) ", node->next_list[i]->i, node->next_list[i]->j);

    fprintf(fp, "\n");
}

void print_lcs_graph(cell_t* entry, const char* output_file) {
    // All functions used below are defined in utils
    lcsQueue* q = initialQueue();
    enQueue(q, entry);

    FILE* fp = fopen(output_file, "w");
    while (q->front) {
        fprint_node(fp, q->front->cell);
        for (int i = 0; i < q->front->cell->next_num; ++i) {
            cell_t* newCell = q->front->cell->next_list[i];
            if (!newCell->visited)
                enQueue(q, newCell);
        }
        deQueue(q, true);
    }

    fclose(fp);
}

void sequentialRecursion(FILE* fp, char* lcs, cell_t* cell) {
    if (cell->next_num) {
        if (cell->match)
            lcs[cell->rank - 1] = cell->match;
        for (int i = 0; i < cell->next_num; ++i)
            sequentialRecursion(fp, lcs, cell->next_list[i]);
    }
    else
        fprintf(fp, "%s\n", lcs);
}

void output_all_lcs_seq(cell_t* entry, const char* output_file) {
    int lcsLen = entry->rank;
    char* lcs = (char*)calloc(lcsLen, sizeof(char));
    FILE* fp = fopen(output_file, "w");
    sequentialRecursion(fp, lcs, entry);
    fclose(fp);
}

void parallelOutput(FILE** fpList, char** lcsTLS, lcsNode* node) {
    int indexWorker = __cilkrts_get_worker_number();
    FILE* fp = fpList[indexWorker];
    char* lcs = lcsTLS[indexWorker];

    lcsNode* parentNode = node->parent;
    while (parentNode) {
        if (parentNode->cell->match)
            lcs[parentNode->cell->rank - 1] = parentNode->cell->match;
        parentNode = parentNode->parent;
    }

    sequentialRecursion(fp, lcs, node->cell);
}

void output_all_lcs_parallel(cell_t* entry, const char* output_folder) {
    int numWorkers = __cilkrts_get_nworkers();
    int lcsLen = entry->rank;

    FILE** fpList = (FILE**)calloc(numWorkers, sizeof(FILE*));
    char** lcsTLS = (char**)calloc(numWorkers, sizeof(char*));
    for (int i = 0; i < numWorkers; ++i) {
        char* index = (char*)calloc((numWorkers / 10 + 5), sizeof(char));
        sprintf(index, "%d", i);
        char* filename = (char*)calloc((strlen(output_folder) + numWorkers / 10 + 10), sizeof(char));
        strcpy(filename, output_folder);
        strcat(filename, "/");
        strcat(filename, index);
        strcat(filename, ".txt");

        fpList[i] = fopen(filename, "w");
        lcsTLS[i] = (char*)calloc(lcsLen, sizeof(char));
    }

    lcsQueue* q = initialQueue();
    enQueue(q, entry);
    int numQNode = 1;
    while ((q->front->cell->rank > 1) && (numQNode < numWorkers)) {
        for (int i = 0; i < q->front->cell->next_num; ++i) {
            cell_t* newCell = q->front->cell->next_list[i];
            enQueue(q, newCell);
        }
        numQNode += q->front->cell->next_num - 1;
        deQueue(q, false);
    }

    for (lcsNode* node = q->front; node != q->rear; node = node->next)
        cilk_spawn parallelOutput(fpList, lcsTLS, node);
    parallelOutput(fpList, lcsTLS, q->rear);
    cilk_sync;

    for (int i = 0; i < numWorkers; ++i)
        fclose(fpList[i]);
}
