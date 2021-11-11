#include "utils.hpp"
#include <cstdlib>
#include <cassert>
#include <string>
#include <queue>
#include <cstdlib>
#include <stack>
#include <cstring>
#include <cilk/cilk.h>
#include <alloca.h>
#include <cilk/cilkscale.h>
#include <cilk/cilksan.h>

int max(const int a, const int b)   
{
    return (a > b) ? a : b;
}

 int min(const int a, const int b)
{
    return (a > b) ? b : a;
}

int naive_lcs_2d(const std::string A, const std::string B)
{
    const int M = A.length();
    const int N = B.length();

    int * dp = (int *)calloc(M * N, sizeof(int));

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int up = (i > 0) ? dp[(i - 1) * N + j] : 0;
            int left = (j > 0) ? dp[i * N + j - 1] : 0;
            int upleft = (i > 0 && j > 0) ? dp[(i - 1) * N + j - 1] : 0;

            if (A[i] == B[j])
                dp[i * N + j] = upleft + 1;
            else 
                dp[i * N + j] = max(up, left);
        }
    }
    return dp[M * N - 1];
}

int _count_lcs_child(lcs_block_t * block)
{
    lcs_block_t * head = block->head;
    lcs_block_t * pretail = block->pretail;
    lcs_block_t * tail = block->tail;
    
    if(head == NULL)
    {
        if (tail == NULL)
            return 0;
        else return 1;
    }

    int count = 2;
    while(head != pretail)
    {
        count++;
        head = head->next;
    }

    return count;

}

lcs_node_t * _init_entry(const std::string & A, const std::string & B)
{
    int const M = A.length();
    int const N = B.length();

    lcs_node_t * entry = (lcs_node_t *)calloc(1, sizeof(lcs_node_t));
    entry->i = M - 1; entry->j = N - 1; 
    entry->match = (A[M-1] == B[N-1]) ? A[M-1] : '\0'; 

    return entry;
}

const lcs_node_t * construct_lcs_graph(const std::string & A, const std::string & B, lcs_block_t * dp)
{
    int const M = A.length();
    int const N = B.length();
    
    lcs_node_t ** table = (lcs_node_t **)calloc(M * N, sizeof(lcs_node_t *));
    std::queue<lcs_node_t*> q;

    lcs_node_t * entry = _init_entry(A, B);
    table[M * N - 1] = entry;
    q.push(entry);

    while(!q.empty())
    {
        lcs_node_t * node = q.front(); q.pop();
        int i = node->i, j = node->j;

        // match
        if (A[i] == B[j])
        {
            int next_i = i-1, next_j = j-1;
            
            // base case
            if (dp[i * N + j].lcs_len <= 1)
            {
                node->next_num = 0; 
                node->next_list = NULL;
#ifdef UTILS_DEBUG
                goto PRINT_INFO;
#else
                continue;
#endif
            }
            // common case
            node->next_num = 1;
            lcs_node_t ** next_list = (lcs_node_t **)calloc(1, sizeof(lcs_node_t *));
            if (table[next_i * N + next_j])
            {   
                // if the next node is already created, we take it from the table of all nodes.
                next_list[0] = table[next_i * N + next_j];
            }
            else
            {
                // otherwise, we create a new node and push it into the queue.
                lcs_node_t * next = (lcs_node_t *)calloc(1, sizeof(lcs_node_t));
                next_list[0] = table[next_i * N + next_j] = next;
                next->i = next_i; next->j = next_j;
                next->match = (A[next_i] == B[next_j]) ? A[next_i] : '\0';
                q.push(next);
            }
            node->next_list = (const lcs_node_t **)next_list;
        }
        // clash
        else
        {
            lcs_block_t * block = dp + i * N + j;
            node->next_num = _count_lcs_child(block);

            // It should be greater than zero
            if (node->next_num > 0)
            {
                node->next_list = (const lcs_node_t **)calloc(node->next_num, sizeof(lcs_node_t *));
                
                lcs_block_t * ptr = (node->next_num > 1) ? block->head : block->tail;
                int c = 0;
                while(ptr != NULL)
                {
                    int next_i = (ptr - dp) / N, next_j = (ptr - dp) % N;
                    if (table[next_i * N + next_j])
                    {
                        node->next_list[c++] = table[next_i * N + next_j];
                    }
                    else
                    {
                        lcs_node_t * next = (lcs_node_t *)calloc(1, sizeof(lcs_node_t));
                        next->i = next_i; next->j = next_j;
                        next->match = (A[next->i] == B[next->j]) ? A[next->i] : '\0';

                        node->next_list[c++] = table[next->i * N + next->j] = next;
                        q.push(next);
                    }

                    if (ptr == block->tail)
                        ptr = NULL;
                    else if (ptr == block->pretail)
                        ptr = block->tail;
                    else ptr = ptr->next;
                }
                if (c != node->next_num){
                    assert(false);
                }
            }
            else
            {
                assert(false);
            }
        }
#ifdef UTILS_DEBUG
PRINT_INFO:
        printf("[%d,%d]", node->i, node->j);
        if (node->match == '\0')
            printf("('\\0')");
        else printf("(%c)", node->match);
        if (node->next_num > 0)
            printf(": ");
        for(int c = 0; c < node->next_num; c++)
        {
            const lcs_node_t * child = node->next_list[c];
            printf("[%d,%d] ", child->i, child->j);
        }
        printf("\n");
#endif
    }

    free(table);

    return entry;
}

const lcs_node_t * lcs_anticanonical(const std::string & A, const std::string & B)
{
    int const M = A.length();
    int const N = B.length();

    lcs_block_t * dp = (lcs_block_t *)calloc(M * N, sizeof(lcs_block_t));

    const lcs_block_t empty_block = {0, NULL, NULL, NULL};
    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
        {
            const lcs_block_t * up = (i > 0) ? (dp + (i-1) * N + j) : &empty_block;
            const lcs_block_t * left = (j > 0) ? (dp + i * N + j - 1) : &empty_block;
            const lcs_block_t * upleft = (i > 0 && j > 0) ? (dp + (i-1) * N + j -1) : &empty_block;

            int up_v = up->lcs_len;
            int left_v = left->lcs_len;
            int upleft_v = upleft->lcs_len;
            lcs_block_t * cur = (dp + i * N + j);

            if (A[i] == B[j])
            {
                cur->lcs_len = upleft_v + 1;
                cur->tail = cur;
            }
            else if (max(up_v, left_v) == 0)
            {
                ; // nothing to do
            }
            else if (up_v > left_v)
            {
                cur->lcs_len = up_v;
                cur->tail = up->tail;
            }
            else if (up_v < left_v)
            {
                cur->lcs_len = left_v;
                cur->tail = left->tail;
            }
            else
            {
                cur->lcs_len = up_v;
                cur->tail = up->tail;
                
                /* tail[i, j − 1] and tail[i - 1 , j] point to same row */
                int up_tail_i = (up->tail - dp) / N;
                int left_tail_i = (left->tail - dp) / N;
                cur->pretail = (up_tail_i == left_tail_i) ? left->pretail : left->tail;
                
                /* pretail[i, j] and tail[i, j] point to same column */
                int cur_pretail_j = (cur->pretail == NULL) ? -1 : (cur->pretail - dp) % N;
                int cur_tail_j = (cur->tail - dp) % N;
                if (cur_pretail_j == cur_tail_j)
                {
                    cur->tail = cur->pretail;
                }
                else
                {
                    if (cur->pretail)
                        cur->pretail->next = cur->tail;
                    cur->head = (left->head != NULL) ? left->head : cur->pretail;
                }
            }
        }
    }
#ifdef UTILS_DEBUG
    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
        {
            lcs_block_t * cur = (dp + i * N + j);

            printf("%d(", cur->lcs_len);
            if (cur->head == NULL)
                printf("h[X],");
            else printf("h[%d,%d],", (int)(cur->head - dp) / N, (int)(cur->head - dp) % N);

            if (cur->tail == NULL)
                printf("t[X],");
            else printf("t[%d,%d],", (int)(cur->tail - dp) / N, (int)(cur->tail - dp) % N);
            
            if (cur->pretail == NULL)
                printf("p[X],");
            else printf("p[%d,%d],", (int)(cur->pretail - dp) / N, (int)(cur->pretail - dp) % N);

            if (cur->next == NULL)
                printf("n[X])\t");
            else printf("n[%d,%d])\t", (int)(cur->next - dp) / N, (int)(cur->next - dp) % N);
        }
        printf("\n");
    }
    printf("\n");
#endif
    const lcs_node_t * entry = construct_lcs_graph(A, B, dp);
    free(dp);

    return entry;
}
