#include <stdio.h>

const int MAX_NODES = 1 << 14;

int n, mask;
char state[MAX_NODES];
int st[2 * MAX_NODES];

void read_data() {
  scanf("%d", &n);
  mask = (1 << (n - 1)) - 1;
}

void euler_tour() {
  if (n == 1) {
    printf("01\n");
    return;
  }

  int size = 1;
  st[0] = 0;

  while (size) {
    int u = st[size - 1];
    if (state[u] == 2) {
      putchar('0' + (u & 1));
      size--;
    } else {
      st[size++] = ((u << 1) + state[u]) & mask;
      state[u]++;
    }
  }

  for (int i = 0; i < n - 2; i++) {
    putchar('0');
  }
  putchar('\n');
}

int main() {
  read_data();
  euler_tour();
  return 0;
}
