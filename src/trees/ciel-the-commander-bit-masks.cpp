#include <stdio.h>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  char rank; // răspunsul
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

// Returnează indicele celui mai din dreapta bit care respectă regulile de
// combinare.
int get_valid_bit(unsigned once, unsigned twice) {
  // Ia cel mai mare rang care apare de două ori.
  int msb = twice
    ? (31 - __builtin_clz(twice))
    : -1;

  // Fiecare bit în stînga lui msb este OK.
  unsigned legal_twice = ~((1 << (msb + 1)) - 1);

  // Fiecare bit văzut o singură dată nu este OK.
  unsigned legal_once = ~once;

  // Trebuie să respectăm ambele criterii.
  unsigned both = legal_once & legal_twice;

  // Returnează indicele celui mai din dreapta bit.
  return __builtin_ctz(both);
}

// Returnează o mască pe 26 de biți a rangurilor vizibile (MSB este A, LSB
// este Z).
unsigned dfs(int u, int parent) {
  unsigned once = 0, twice = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      unsigned vis_mask = dfs(v, u);
      // Noduri văzute o dată înainte și a doua oară în subarborele lui v.
      twice |= vis_mask & once;
      once |= vis_mask;
    }
  }

  int bit = get_valid_bit(once, twice);
  nd[u].rank = 'Z' - bit;

  // Acest rang este vizibil și maschează toate rangurile mai mici.
  return (1 << bit) | (once & ~((1 << bit) - 1));
}

void write_ranks() {
  for (int u = 1; u <= n; u++) {
    putchar(nd[u].rank);
    putchar((u == n) ? '\n' : ' ');
  }
}

int main() {
  read_input_data();
  dfs(1, 0);
  write_ranks();

  return 0;
}
