#include "utils.hpp"
#include <string.h>
#include <cilk/cilk.h>


int lcs_basic(const char *_A, const char *_B, int M, int N)
{
	int lcsLen;	
	int* dp = (int*)calloc(N, sizeof(int));


	for (int i = 0; i < M; ++i) {
		int upleft = dp[0];
		if (_A[i] == _B[0])
			dp[0] = 1;
		
		for (int j = 1; j < N; ++j) {
			int up = dp[j], left = dp[j - 1];
			int cur;

			if (_A[i] == _B[j])
				cur = upleft + 1;
			
			else if (up > left)
				cur = up;

			else
				cur = left;

			upleft = dp[j];
			dp[j] = cur;
		}
	}

	lcsLen = dp[N - 1];
	free(dp);

	return lcsLen;
}

int lcs_ad(const char * _A, const char * _B, int M, int N)
{
	const char* A = _A;
	const char* B = _B;
	int lcsLen;
	
	// M = min(M, N), which is the length of subdiagonal
	if (M > N) {
		// Swap M and N
		N = M ^ N;
		M = M ^ N;
		N = M ^ N;

		// A must be shorter than B
		A = _B;
		B = _A;
	}

	int* dp_now = (int*)calloc(M, sizeof(int));
	int* dp_pre = (int*)calloc(M, sizeof(int));
	bool flag1 = false, flag2 = false;

	for (int i = 0; i < M; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		// Update dp_now[0] and dp_now[i]
		int upleft = dp_now[0];
		if (flag1)
			dp_now[0] = 1;
		else if (A[i] == B[0]) {
			dp_now[0] = 1;
			flag1 = true;
		}
		if (flag2)
			dp_now[i] = 1;
		else if (A[0] == B[i]) {
			dp_now[i] = 1;
			flag2 = true;
		}

		for (int j = 1; j < i; ++j) {
			int up = dp_pre[j], left = dp_pre[j - 1];
			int cur;

			if (A[i - j] == B[j])
				cur = upleft + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			upleft = dp_now[j];
			dp_now[j] = cur;
		}
	}

	for (int k = M - 1; k >= 0; --k)
		dp_pre[k] = dp_pre[k - 1];

	for (int i = M; i < N; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		for(int j = 0; j < M - 1; ++j) {
			int up = dp_pre[j + 1], left = dp_pre[j];
			int cur;

			if (A[M - 1 - j] == B[i + j - M + 1])
				cur = dp_now[j + 1] + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			dp_now[j] = cur;
		}

		// Update dp_now[M]
		if (flag2)
			dp_now[M] = 1;
		else if (A[0] == B[i]) {
			dp_now[M] = 1;
			flag2 = true;
		}
	}

	for (int i = N; i < N + M - 1; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		// No need to update dp_now

		for (int j = 0; j < N + M - 1 - i; ++j) {
			int up = dp_pre[j + 1], left = dp_pre[j];
			int cur;

			if (A[M - 1 - j] == B[i + j - M + 1])
				cur = dp_now[j + 1] + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			dp_now[j] = cur;
		}
	}

	lcsLen = dp_now[0];
	free(dp_now);
	free(dp_pre);

	return lcsLen;
}

int lcs_ad_parallel(const char * _A, const char * _B, int M, int N)
{
    const char* A = _A;
	const char* B = _B;
	int lcsLen;
	
	// M = min(M, N), which is the length of subdiagonal
	if (M > N) {
		// Swap M and N
		N = M ^ N;
		M = M ^ N;
		N = M ^ N;

		// A must be shorter than B
		A = _B;
		B = _A;
	}

	int* dp_now = (int*)calloc(M, sizeof(int));
	int* dp_pre = (int*)calloc(M, sizeof(int));
	bool flag1 = false, flag2 = false;

	for (int i = 0; i < M; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		// Update dp_now[0] and dp_now[i]
		int upleft = dp_now[0];
		if (flag1)
			dp_now[0] = 1;
		else if (A[i] == B[0]) {
			dp_now[0] = 1;
			flag1 = true;
		}
		if (flag2)
			dp_now[i] = 1;
		else if (A[0] == B[i]) {
			dp_now[i] = 1;
			flag2 = true;
		}

		cilk_for (int j = 1; j < i; ++j) {
			int up = dp_pre[j], left = dp_pre[j - 1];
			int cur;

			if (A[i - j] == B[j])
				cur = upleft + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			upleft = dp_now[j];
			dp_now[j] = cur;
		}
	}

	for (int k = M - 1; k >= 0; --k)
		dp_pre[k] = dp_pre[k - 1];

	for (int i = M; i < N; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		cilk_for(int j = 0; j < M - 1; ++j) {
			int up = dp_pre[j + 1], left = dp_pre[j];
			int cur;

			if (A[M - 1 - j] == B[i + j - M + 1])
				cur = dp_now[j + 1] + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			dp_now[j] = cur;
		}

		// Update dp_now[M]
		if (flag2)
			dp_now[M] = 1;
		else if (A[0] == B[i]) {
			dp_now[M] = 1;
			flag2 = true;
		}
	}

	for (int i = N; i < N + M - 1; ++i) {
		// Swap dp_now and dp_pre
		int* dp_tmp = dp_now;
		dp_now = dp_pre;
		dp_pre = dp_tmp;

		// No need to update dp_now

		cilk_for (int j = 0; j < N + M - 1 - i; ++j) {
			int up = dp_pre[j + 1], left = dp_pre[j];
			int cur;

			if (A[M - 1 - j] == B[i + j - M + 1])
				cur = dp_now[j + 1] + 1;

			else if (up > left)
				cur = up;

			else
				cur = left;

			dp_now[j] = cur;
		}
	}

	lcsLen = dp_now[0];
	free(dp_now);
	free(dp_pre);

	return lcsLen;
}
