#include <stdio.h>

const int MAX_N = 1'000'000;

int phi[MAX_N + 1];

int read_n() {
  int n;
  FILE* f = fopen("emptri.in", "r");
  fscanf(f, "%d", &n);
  fclose(f);
  return n;
}

void compute_phi(int n) {
  for (int i = 1; i <= n; i++) {
    phi[i] = i;
  }
  for (int i = 2; i <= n; i++) {
    if (phi[i] == i) {
      for (int j = i; j <= n; j += i) {
        phi[j] -= phi[j] / i;
      }
    }
  }
}

long long sum_phi(int n) {
  long long sum = 0;
  for (int i = 2; i <= n; i++) {
    sum += phi[i];
  }
  return sum * 2 + 1;
}

void write_answer(long long answer) {
  FILE* f = fopen("emptri.out", "w");
  fprintf(f, "%lld\n", answer);
  fclose(f);
}

int main() {
  int n = read_n();
  compute_phi(n);
  long long answer = sum_phi(n);
  write_answer(answer);

  return 0;
}
