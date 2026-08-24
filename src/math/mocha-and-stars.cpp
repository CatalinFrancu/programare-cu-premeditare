#include <stdio.h>

const int MAX_N = 50;
const int MAX_VAL = 100'000;
const int MOD = 998'244'353;

struct knapsack {
  int v[2][MAX_VAL + 1];
  int row;
  int max_val;

  void clear_row(int r) {
    for (int i = 0; i <= max_val; i++) {
      v[r][i] = 0;
    }
  }

  void partial_sums(int r) {
    for (int i = 1; i <= max_val; i++) {
      v[r][i] = (v[r][i] + v[r][i - 1]) % MOD;
    }
  }

  void init(int _max_val) {
    row = 0;
    max_val = _max_val;
    clear_row(0);
    v[0][0] = 1;
  }

  void add_range(int lo, int hi) {
    clear_row(1 - row);

    // Construiește vectorul de diferențe.
    for (int i = 0; i <= max_val - lo; i++) {
      v[1 - row][i + lo] = (v[1 - row][i + lo] + v[row][i]) % MOD;
      if (i + hi + 1 <= max_val) {
        v[1 - row][i + hi + 1] = (v[1 - row][i + hi + 1] + MOD - v[row][i]) % MOD;
      }
    }

    partial_sums(1 - row);
    row = 1 - row;
  }

  int sum_all() {
    long long sum = 0;
    for (int i = 0; i <= max_val; i++) {
      sum += v[row][i];
    }
    return sum % MOD;
  }
};

int lo[MAX_N], hi[MAX_N];
int mu[MAX_VAL + 1];
knapsack kn;
int n, m;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &lo[i], &hi[i]);
  }
}

void compute_mobius() {
  mu[1] = 1;
  for (int i = 1; i <= m; i++) {
    if (mu[i]) {
      for (int j = 2 * i; j <= m; j += i) {
        mu[j] -= mu[i];
      }
    }
  }
}

int count_partitions(int d) {
  kn.init(m / d);
  for (int i = 0; i < n; i++) {
    kn.add_range((lo[i] - 1) / d + 1, hi[i] / d);
  }
  return kn.sum_all();
}

int sum_over_divisors() {
  long long answer = 0;

  for (int d = 1; d <= m; d++) {
    if (mu[d]) {
      answer += (long long)mu[d] * count_partitions(d);
    }
  }

  answer %= MOD;
  if (answer < 0) {
    answer += MOD;
  }

  return answer % MOD;
}

int main() {
  read_data();
  compute_mobius();
  int answer = sum_over_divisors();
  printf("%d\n", answer);

  return 0;
}
