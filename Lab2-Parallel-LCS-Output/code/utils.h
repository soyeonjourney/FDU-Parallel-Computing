typedef struct _cell {
    int i, j;
    int rank;
    struct _cell* tail;
    struct _cell* pretail;
    struct _cell* head;
    struct _cell* next;
    int visited;
    int match;
    int next_num;
    struct _cell** next_list;
} cell_t;

typedef struct _lcs_node {
    cell_t* cell;
    _lcs_node* parent;
    _lcs_node* next;
} lcsNode;

typedef struct _lcs_queue {
    lcsNode* front;
    lcsNode* rear;
} lcsQueue;

#define SET_STRUCT(__type__, __lval__, ...)     \
    do {                                        \
        __type__ __rval__ = {__VA_ARGS__};      \
        __lval__ = __rval__;                    \
    } while(0)

// #define ANTICANONICAL_DEBUG

int naive_lcs_2d(const char* A, const char* B, int M, int N);

cell_t* lcs_anticanonical(const char* A, const char* B, int M, int N);

lcsNode* newNode(cell_t* cell);

lcsQueue* initialQueue();

void enQueue(lcsQueue* q, cell_t* cell);

void deQueue(lcsQueue* q, bool isFree);
