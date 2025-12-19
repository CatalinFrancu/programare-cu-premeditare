#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;   // începutul listei de adiacență
  int size;  // mărimea subarborelui, inclusiv nodul însuși
};

cell list[MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  scanf("%d",  &n);
  for (int u = 2; u <= n; u++) {
    int par;
    scanf("%d", &par);
    add_child(par, u);
  }
}

void depth_first_search(int u) {
  nd[u].size = 1;
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    depth_first_search(v);
    nd[u].size += nd[v].size;
  }
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    printf("%d ", nd[u].size - 1);
  }
  printf("\n");
}

int main() {
  read_data();
  depth_first_search(1);
  write_answers();

  return 0;
}
