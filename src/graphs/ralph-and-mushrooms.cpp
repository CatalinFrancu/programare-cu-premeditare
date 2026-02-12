#include <math.h>
#include <stdio.h>

const int MAX_NODES = 1'000'000;
const int MAX_EDGES = 1'000'000;

typedef unsigned long long u64;

struct edge {
  int v, mushrooms, next;
};

struct node {
  u64 inc_profit, exc_profit; // profitul cu/fără CTC-ul lui u
  int adj;
  int time_in;
  int low;
  bool on_stack;
};

node nd[MAX_NODES + 1];
edge e[MAX_EDGES + 1];
int st[MAX_NODES], ss;  // stiva DFS
int n, start_node;

void read_graph() {
  int num_edges, u;
  scanf("%d %d", &n, &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    scanf("%d %d %d", &u, &e[i].v, &e[i].mushrooms);
    e[i].next = nd[u].adj;
    nd[u].adj = i;
  }
  scanf("%d", &start_node);
}

u64 max(u64 x, u64 y) {
  return (x > y) ? x : y;
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

u64 get_mushroom_profit(int m) {
  // Implementează https://oeis.org/A060432. Remarcăm că diferențele sînt 1,
  // 2, 2, 3, 3, 3, 4, 4, 4, 4 etc. De aceea, rezolvă t * (t + 1) / 2 ≤ m.
  // Ultimul termen cu un t întreg este o sumă de pătrate.
  int t = (sqrtl(8 * m + 1) - 1) * 0.5;
  u64 base = t * (t + 1) / 2;
  u64 sum = base * (2 * t + 1) / 3;
  sum += (u64)(m - base) * (t + 1);
  return sum;
}

void update_profits(int u, int v, int mushrooms) {
  if (nd[v].on_stack) {
    // muchie internă
    u64 p = get_mushroom_profit(mushrooms);
    nd[u].exc_profit = max(nd[u].exc_profit, nd[v].exc_profit);
    if (u == v) {
      nd[u].inc_profit += p;
    } else {
      nd[u].inc_profit += p + nd[v].inc_profit;
      nd[v].inc_profit = 0;
    }
  } else {
    // muchie externă
    nd[u].exc_profit = max(nd[u].exc_profit, nd[v].inc_profit + mushrooms);
  }
}

void pop_scc(int root) {
  u64 scc_profit = nd[root].inc_profit + nd[root].exc_profit;

  int v;
  do {
    v = st[--ss];
    nd[v].on_stack = false;
    nd[v].inc_profit = scc_profit;
  } while (v != root);
}

void tarjan(int u) {
  static int time = 0;

  nd[u].time_in = nd[u].low = ++time;
  nd[u].on_stack = true;
  st[ss++] = u;

  for (int ptr = nd[u].adj; ptr; ptr = e[ptr].next) {
    int v = e[ptr].v;

    if (!nd[v].time_in) {
      tarjan(v);
      nd[u].low = min(nd[u].low, nd[v].low);
    } else if (nd[v].on_stack) {
      nd[u].low = min(nd[u].low, nd[v].time_in);
    }

    update_profits(u, v, e[ptr].mushrooms);
  }

  if (nd[u].low == nd[u].time_in) {
    pop_scc(u);
  }
}

void write_answer() {
  printf("%llu\n", nd[start_node].inc_profit);
}

int main() {
  read_graph();
  tarjan(start_node);
  write_answer();

  return 0;
}
