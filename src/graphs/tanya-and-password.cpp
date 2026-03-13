#include <stdio.h>

const int MAX_LEN = 200'000;
const int BITS = 6;
const int BASE = 1 << BITS;
const int MAX_NODES = BASE * BASE;
const int NUM_RANGES = 3;
const int RANGES[NUM_RANGES][2] = {{'0', '9'}, {'A', 'Z'}, {'a', 'z'}};

struct node {
  int adj[BASE]; // vector de adiacență
  int ptr;       // prima valoare nenulă din adj
  int in, out;   // gradele la intrare / ieșire
};

node nd[MAX_NODES];
int len;
unsigned char rank[128], unrank[BASE];
short sol[MAX_LEN + 2];
short st[MAX_LEN + 2];

void make_tables() {
  int d = 1;

  for (int r = 0; r < NUM_RANGES; r++) {
    for (int c = RANGES[r][0]; c <= RANGES[r][1]; c++) {
      rank[c] = d++;
      unrank[rank[c]] = c;
    }
  }
}

int make_node(int x, int y) {
  return x * BASE + y;
}

int upper(int u) {
  return u >> BITS;
}

int lower(int u) {
  return u & (BASE - 1);
}

void add_edge(int x, int y, int z) {
  int u = make_node(x, y);
  int v = make_node(y, z);
  nd[u].adj[z]++;
  nd[u].out++;
  nd[v].in++;
}

void read_graph() {
  scanf("%d ", &len);
  for (int i = 0; i < len; i++) {
    unsigned char a, b, c;
    scanf("%c%c%c ", &a, &b, &c);
    // Răsturnat, deoarece algoritmul generează circuitul răsturnat.
    add_edge(rank[c], rank[b], rank[a]);
  }
}

int get_start_node() {
  int num_zero = 0, num_one = 0;
  int start_node = 0;

  for (int u = 0; u < MAX_NODES; u++) {
    if (nd[u].out == nd[u].in) {
      num_zero++;
    }
    if (nd[u].out == nd[u].in + 1) {
      num_one++;
      start_node = u;
    }
    if (nd[u].out && !start_node) {
      start_node = u;
    }
  }

  if ((num_zero == MAX_NODES) ||
      ((num_zero == MAX_NODES - 2) && (num_one == 1))) {
    return start_node;
  } else {
    return 0;
  }
}

int find_neighbor(int u) {
  int& v = nd[u].ptr;
  while ((v < BASE) && !nd[u].adj[v]) {
    v++;
  }

  if (v < BASE) {
    nd[u].adj[v]--;
    return make_node(lower(u), v);
  } else {
    return 0;
  }
}

bool euler_tour() {
  int start_node = get_start_node();
  if (!start_node) {
    return false;
  }

  int st_size = 1, sol_size = 0;
  st[0] = start_node;

  while (st_size) {
    int u = st[st_size - 1];
    int v = find_neighbor(u);
    if (v) {
      st[st_size++] = v;
    } else {
      sol[sol_size++] = u;
      st_size--;
    }
  }

  return (sol_size == len + 1);
}

void write_solution() {
  printf("YES\n");
  for (int i = 0; i <= len; i++) {
    putchar(unrank[lower(sol[i])]);
  }
  putchar(unrank[upper(sol[len])]);
  putchar('\n');
}

int main() {
  make_tables();
  read_graph();
  bool feasible = euler_tour();

  if (feasible) {
    write_solution();
  } else {
    printf("NO\n");
  }

  return 0;
}
