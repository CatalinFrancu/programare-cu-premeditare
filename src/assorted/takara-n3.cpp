#include <algorithm>
#include <stdio.h>

const int MAX_N = 5'000;

int n;
int c[MAX_N];
int d[MAX_N][MAX_N];

void read_data() {
  FILE *f = fopen("takara.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d", &c[i]);
  }
  fclose(f);
}

void compute_costs() {
  for (int i = n - 1; i >= 0; i--) {
    for (int j = i + 1; j < n; j++) {
      d[i][j] = std::min(c[i] + d[i + 1][j], c[j] + d[i][j - 1]);
      for (int k = i + 1; k < j; k++) {
        d[i][j] = std::min(d[i][j],
                           c[k] + std::max(d[i][k - 1], d[k + 1][j]));
      }
    }
  }
}

void write_result() {
  FILE* f = fopen("takara.out", "w");
  fprintf(f, "%d\n", d[0][n - 1]);
  fclose(f);
}

int main() {
  read_data();
  compute_costs();
  write_result();

  return 0;
}
