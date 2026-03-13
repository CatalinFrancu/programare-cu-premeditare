#include <stdio.h>

const int MAX_NODES = 1 << 14;

int n, mask;
char state[MAX_NODES];

void read_data() {
  scanf("%d", &n);
  mask = (1 << (n - 1)) - 1;
}

void euler_tour(int u) {
  while (state[u] < 2) {
    int v = ((u << 1) + state[u]) & mask;
    state[u]++;
    euler_tour(v);
  }

  putchar('0' + (u & 1));
}

void euler_tour_wrapper() {
  if (n == 1) {
    printf("01\n");
    return;
  }

  euler_tour(0);

  for (int i = 0; i < n - 2; i++) {
    putchar('0');
  }
  putchar('\n');
}

int main() {
  read_data();
  euler_tour_wrapper();
  return 0;
}
