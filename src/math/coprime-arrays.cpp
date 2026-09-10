#include <stdio.h>

const int MAX_VAL = 2'000'000;
const int MOD = 1'000'000'007;

int mu[MAX_VAL + 1];
int pow_n[MAX_VAL + 1];
long long changes[MAX_VAL + 1];
int n, k;

void read_data() {
  scanf("%d %d", &n, &k);
}

int bin_exp(int b, int e) {
  long long result = 1;

  while (e) {
    if (e & 1) {
      result = result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

void compute_powers() {
  for (int i = 1; i <= k; i++) {
    pow_n[i] = bin_exp(i, n);
  }
}

void compute_mobius(int n) {
  for (int i = 1; i <= n; i++) {
    mu[i] = 1;
  }

  for (int i = 2; i * i <= n; i++) {
    if (mu[i] == 1) {
      for (int j = i; j <= n; j += i) {
        mu[j] *= -i;
      }
      for (int j = i * i; j <= n; j += i * i) {
        mu[j] = 0;
      }
    }
  }

  for (int i = 2; i <= n; i++) {
    if (mu[i] == i) {
      mu[i] = 1;
    } else if (mu[i] == -i) {
      mu[i] = -1;
    } else if (mu[i] < 0) {
      mu[i] = 1;
    } else if (mu[i] > 0) {
      mu[i] = -1;
    }
  }
}

void compute_changes() {
  for (int div = 1; div <= k; div++) {
    for (int quot = 1, mult = div; mult <= k; quot++, mult += div) {
      changes[mult] += (pow_n[quot] - pow_n[quot - 1]) * mu[div];
    }
  }
}

void count_arrays() {
  long long cnt = 0;
  long long answer = 0;
  for (int i = 1; i <= k; i++) {
    cnt = (cnt + changes[i]) % MOD;
    if (cnt < 0) {
      cnt = MOD + cnt;
    }

    answer += cnt ^ i;
  }
  printf("%lld\n", answer % MOD);
}

int main() {
  read_data();
  compute_powers();
  compute_mobius(k);
  compute_changes();
  count_arrays();

  return 0;
}
