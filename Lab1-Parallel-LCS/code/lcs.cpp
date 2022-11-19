#include <cilk/cilk.h>

#include "utils.hpp"

int lcs_basic(const char* _A, const char* _B, int M, int N) {
    int* dp = (int*)calloc(N, sizeof(int));

    for (int i = 0; i < M; ++i) {
        int upleft = dp[0];
        if (_A[i] == _B[0]) dp[0] = 1;

        for (int j = 1; j < N; ++j) {
            int up = dp[j], left = dp[j - 1];
            int cur;

            if (_A[i] == _B[j])
                cur = upleft + 1;

            else
                cur = (up > left) ? up : left;

            upleft = dp[j];
            dp[j] = cur;
        }
    }

    free(dp);
    return dp[N - 1];
}

int lcs_ad(const char* _A, const char* _B, int M, int N) {
    int* dp = (int*)calloc(M + N + 1, sizeof(int));
    int idxMin, idxMax, up, left;

    for (int i = 2; i <= M + N; ++i) {
        idxMin = (i <= M) ? (M - i + 2) : (i - M);
        idxMax = (i <= N) ? (M + i - 2) : (2 * N + M - i);
        for (int j = idxMin; j <= idxMax; j += 2) {
            if (_A[(i - j + M - 2) / 2] == _B[(i + j - M - 2) / 2])
                ++dp[j];
            else {
                up = dp[j + 1];
                left = dp[j - 1];
                dp[j] = (up > left) ? up : left;
            }
        }
    }

    free(dp);
    return dp[N];
}

int lcs_ad_parallel(const char* _A, const char* _B, int M, int N) {
    int* dp = (int*)calloc(M + N + 1, sizeof(int));
    int idxMin, idxMax, up, left;

    for (int i = 2; i <= M + N; ++i) {
        idxMin = (i <= M) ? (M - i + 2) : (i - M);
        idxMax = (i <= N) ? (M + i - 2) : (2 * N + M - i);
#pragma grainsize = 2048
        cilk_for(int j = idxMin; j <= idxMax; j += 2) {
            if (_A[(i - j + M - 2) / 2] == _B[(i + j - M - 2) / 2])
                ++dp[j];
            else {
                up = dp[j + 1];
                left = dp[j - 1];
                dp[j] = (up > left) ? up : left;
            }
        }
    }

    free(dp);
    return dp[N];
}
