#include <stdio.h>

const int MOD = 666'013;

struct matrix {
  long long a, b, c, d;
};

int read() {
  int n;

  FILE* f = fopen("kfib.in", "r");
  fscanf(f, "%d", &n);
  fclose(f);

  return n;
}

void write(int x) {
  FILE* f = fopen("kfib.out", "w");
  fprintf(f, "%d\n", x);
  fclose(f);
}

matrix mult(matrix p, matrix q) {
  return {
    (p.a * q.a + p.b * q.c) % MOD,
    (p.a * q.b + p.b * q.d) % MOD,
    (p.c * q.a + p.d * q.c) % MOD,
    (p.c * q.b + p.d * q.d) % MOD,
  };
}

matrix mod_pow(matrix a, int n) {
  matrix result = { 1, 0, 0, 1 };

  while (n) {
    if (n & 1) {
      result = mult(result, a);
    }
    a = mult(a, a);
    n >>= 1;
  }

  return result;
}

int fib(int n) {
  matrix m = { 1, 1, 1, 0 };
  matrix m_n = mod_pow(m, n - 1);
  return m_n.a;
}

int main() {
  int n = read();
  write(fib(n));

  return 0;
}
