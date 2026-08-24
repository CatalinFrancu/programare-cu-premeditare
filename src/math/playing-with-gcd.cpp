#include <stdio.h>

const int MAX_N = 100'000;

int phi[MAX_N + 1];
long long answer[MAX_N + 1];

void compute_all_phi() {
  for (int i = 0; i <= MAX_N; i++) {
    phi[i] = i;
  }

  for (int i = 2; i <= MAX_N; i++) {
    if (phi[i] == i) {
      for (int j = i; j <= MAX_N; j += i) {
        phi[j] -= phi[j] / i;
      }
    }
  }
}

void compute_all_answers() {
  for (int i = 1; i <= MAX_N; i++) {
    answer[i] = answer[i - 1] + (i - phi[i]);
  }
}

void serve_answers() {
  int num_queries, n;
  scanf("%d", &num_queries);
  for (int q = 1; q <= num_queries; q++) {
    scanf("%d", &n);
    printf("Case %d: %lld\n", q, answer[n]);
  }
}

int main() {
  compute_all_phi();
  compute_all_answers();
  serve_answers();

  return 0;
}
