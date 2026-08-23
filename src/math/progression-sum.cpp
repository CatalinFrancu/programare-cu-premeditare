// Date fiind a și n, calculează 1 + a + a^2 + ... + a^{n-1} modulo MOD.
#include <assert.h>
#include <stdio.h>

const int MAX_A = 100;
const int MAX_N = 100;
const int MOD = 1'000'000'007;

int prog_sum_naive(int a, int n) {
  long long sum = 0;
  long long factor = 1;
  for (int p = 0; p < n; p++) {
    sum += factor;
    factor = factor * a % MOD;
  }
  sum %= MOD;
  return sum;
}

int prog_sum_iterative(int a, int n) {
  int num_bits = 32 - __builtin_clz(n);
  long long sum = 0;
  long long ak = 1;

  for (int i = 0; i < num_bits; i++) {
    // Contract: fie k numărul format din primii i biți ai lui n.
    // Atunci la această linie ak = a^k și sum = 1 + a + ... + a^{k-1}.
    // Din vechiul ak (deci a^{k/2}) calculăm noul sum.
    sum = (1 + ak) * sum % MOD;
    ak = ak * ak % MOD;

    int bit = (n >> (num_bits - i - 1)) & 1;
    if (bit) {
      sum = (1 + a * sum) % MOD;
      ak = ak * a % MOD;
    }
  }

  return sum;
}

// La fel ca prog_sum_iterative(), dar schimbă direcția lui i pentru cod puțin
// mai scurt.
int prog_sum_iterative_2(int a, int n) {
  int num_bits = 32 - __builtin_clz(n);
  long long sum = 0;
  long long ak = 1;

  for (int i = num_bits - 1; i >= 0; i--) {
    sum = (1 + ak) * sum % MOD;
    ak = ak * ak % MOD;

    if ((n >> i) & 1) {
      sum = (1 + a * sum) % MOD;
      ak = ak * a % MOD;
    }
  }

  return sum;
}

int bin_exp(int b, int e) {
  int result = 1;

  while (e) {
    if (e & 1) {
      result = (long long)result * b % MOD;
    }
    b = (long long)b * b % MOD;
    e >>= 1;
  }

  return result;
}

int prog_sum_recursive_slow(int a, int n) {
  if (n == 1) {
    return 1;
  }

  if (n & 1) {
    int rec = prog_sum_recursive_slow(a, n - 1);
    return (1 + (long long) a * rec) % MOD;
  } else {
    long long mult = 1 + bin_exp(a, n / 2);
    return mult * prog_sum_recursive_slow(a, n / 2) % MOD;
  }
}

struct prog {
  long long term; // stochează a^n pentru un n oarecare
  long long sum;  // stochează 1 + a + ... + a^{n-1} pentru același n
};

prog prog_sum_recursive(int a, int n) {
  if (n == 1) {
    return { .term = a, .sum = 1 };
  }

  if (n & 1) {
    prog p = prog_sum_recursive(a, n - 1);
    return {
      p.term * a % MOD,
      (1 + a * p.sum) % MOD,
    };
  } else {
    prog p = prog_sum_recursive(a, n / 2);
    return {
      p.term * p.term % MOD,
      (1 + p.term) * p.sum % MOD,
    };
  }
}

int main() {
  for (int a = 1; a <= MAX_A; a++) {
    for (int n = 1; n <= MAX_N; n++) {
      int naive = prog_sum_naive(a, n);
      assert(naive == prog_sum_iterative(a, n));
      assert(naive == prog_sum_iterative_2(a, n));
      assert(naive == prog_sum_recursive_slow(a, n));
      assert(naive == prog_sum_recursive(a, n).sum);
    }
  }

  return 0;
}
