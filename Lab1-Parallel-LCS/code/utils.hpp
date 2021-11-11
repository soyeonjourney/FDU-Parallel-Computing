#include <string>
#include <cstdlib>
#include <ctime>

#define UTILS_DEBUG

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
    

typedef enum _direction
{
    BASE,
    UPLEFT,
    UP,
    LEFT,
    UP_OR_LEFT
} dir_t;

typedef struct _pair
{
    int x;
    int y;
} pair_t;

#define ASSIGN_PAIR(dst, src) \
    dst.x = src.x, dst.y = src.y

#define PAIR_X_EQ(a, b) \
    a.x == b.x

typedef struct _lcs_block
{
    int lcs_len;
    struct _lcs_block * head;
    struct _lcs_block * tail;
    struct _lcs_block * pretail;
    struct _lcs_block * next;
} lcs_block_t;

typedef struct _lcs_node
{
    int i;
    int j;
    char match;
    int next_num;
    const struct _lcs_node ** next_list; 
} lcs_node_t;


int max(const int a, const int b);
int min(const int a, const int b);

int naive_lcs_2d(const std::string A, const std::string B);