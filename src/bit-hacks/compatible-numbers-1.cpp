// Inspirată de https://codeforces.com/contest/165/submission/290884750
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_BITS = 22;
const int MAX_VALUE = 1 << MAX_BITS;

int a[MAX_N];
int fit[1 << MAX_BITS];
int n;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    fit[a[i]] = a[i];
  }
}

void compute_fits() {
  // Propagă măștile în ordinea bitului, apoi în ordinea măștii.
  for (int bit = 0; bit < MAX_BITS; bit++) {
    for (int mask = 1; mask < MAX_VALUE; mask++) {
      if ((mask & (1 << bit)) && !fit[mask]) {
        fit[mask] = fit[mask ^ (1 << bit)];
      }
    }
  }
}

void write_answers() {
  for (int i = 0; i < n; i++) {
    int neg = (MAX_VALUE - 1) ^ a[i];
    int answer = fit[neg] ? fit[neg] : -1;
    printf("%d ", answer);
  }
  printf("\n");
}

int main() {
  read_data();
  compute_fits();
  write_answers();

  return 0;
}
