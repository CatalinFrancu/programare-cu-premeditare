#include <stdio.h>

const int MAX_N = 300'000;

typedef int permutation[MAX_N + 1];

permutation p, dest, aux;
int n, k;

void read_data() {
  scanf("%d %d", &n, &k);
}

void make_perm() {
  int pos = 1;
  for (int i = 1; i <= n; i += 2) {
    p[pos++] = i;
  }
  for (int i = 2; i <= n; i += 2) {
    p[pos++] = i;
  }
}

void identity(permutation p) {
  for (int i = 1; i <= n; i++) {
    p[i] = i;
  }
}

// p[i] <- p[q[i]]
void mult(permutation p, permutation q) {
  for (int i = 1; i <= n; i++) {
    aux[i] = p[q[i]];
  }
  for (int i = 1; i <= n; i++) {
    p[i] = aux[i];
  }
}

void bin_exp(permutation a, int k, permutation dest) {
  identity(dest);

  while (k) {
    if (k & 1) {
      mult(dest, a);
    }
    mult(a, a);
    k >>= 1;
  }
}

void write_permutation(permutation p) {
  for (int i = 1; i <= n; i++) {
    printf("%d ", p[i]);
  }
  printf("\n");
}

void solve_test() {
  read_data();
  make_perm();
  bin_exp(p, k, dest);
  write_permutation(dest);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
