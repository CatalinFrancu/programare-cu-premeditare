#include <stdio.h>

const int MAX = 5'000;
const int MOD = 1'000'000'007;

int n, m, k;
int t[MAX + 1];
int col_sum[MAX + 1];
int diag_sum[MAX + 1];
int mat[MAX / 2];
int ans[MAX / 2];

void read_data() {
  FILE* f = fopen("zid.in", "r");
  fscanf(f, "%d %d %d", &n, &m, &k);
  fclose(f);
}

void compute_width_1_tower() {
  for (int even = 0; 2 * even <= k; even++) {
    t[0] = t[1] = col_sum[0] = col_sum[1] = (even == 0);
    for (int h = 2; h <= n; h++) {
      t[h] = (col_sum[h - 1] + diag_sum[h - 2]) % MOD;
      col_sum[h] = (t[h] + col_sum[h - 2]) % MOD;
    }

    for (int h = n; h >= 2; h--) {
      diag_sum[h] = (t[h] + diag_sum[h - 2]) % MOD;
    }
    diag_sum[0] = diag_sum[1] = t[0];

    mat[even] = t[n];
  }
}

void col_mult(int* dest, int* src) {
  for (int i = k / 2; i >= 0; i--) {
    __int128 sum = 0;
    for (int j = 0; j <= i; j++) {
      sum += (long long)src[j] * dest[i - j];
    }
    dest[i] = sum % MOD;
  }
}

void compute_width_m_wall() {
  ans[0] = 1;
  while (m) {
    if (m & 1) {
      col_mult(ans, mat);
    }
    col_mult(mat, mat);
    m >>= 1;
  }
}

void write_answer() {
  FILE* f = fopen("zid.out", "w");
  fprintf(f, "%d\n", (k & 1) ? 0 : ans[k / 2]);
  fclose(f);
}

int main() {
  read_data();
  compute_width_1_tower();
  compute_width_m_wall();
  write_answer();

  return 0;
}
