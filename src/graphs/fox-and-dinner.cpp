#include <stdio.h>

const int MAX_N = 200;
const int MAX_NODES = MAX_N + 2;
const int MAX_EDGES = (MAX_N / 2) * (MAX_N / 2) + MAX_N;
const int MAX_VAL = 10'000;
const int NIL = -1;

struct edge {
  short v;
  short cap;
  short next;
};

struct node {
  short adj;
  short parent, edge;
};

struct queue {
  int v[MAX_N + 2];
  int head, tail;

  void clear() {
    head = tail = 0;
  }

  void enqueue(int x) {
    v[tail++] = x;
  }

  int dequeue() {
    return v[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

short age[MAX_N + 1];
bool composite[2 * MAX_VAL + 1];
node nd[MAX_NODES];
edge e[2 * MAX_EDGES]; // include și muchiile reziduale
queue q;
bool used[MAX_N + 1];
unsigned char cycle[MAX_N][MAX_N];
int cycle_len[MAX_N];
int num_cycles;
int n, num_odd, source, sink;
bool possible;

void read_data() {
  scanf("%d", &n);
  for (int i = 1; i <= n; i++) {
    scanf("%hd", &age[i]);
    num_odd += age[i] % 2;
  }
  possible = (2 * num_odd == n);
}

void sieve() {
  for (int i = 2; i <= 2 * MAX_VAL; i++) {
    if (!composite[i]) {
      for (int j = 2 * i; j <= 2 * MAX_VAL; j += i) {
        composite[j] = true;
      }
    }
  }
}

void add_edge(short u, short v, short cap) {
  static int ptr = 0;

  e[ptr] = { v, cap, nd[u].adj };
  nd[u].adj = ptr++;
  e[ptr] = { u, 0, nd[v].adj };
  nd[v].adj = ptr++;
}

void build_graph() {
  source = 0;
  sink = n + 1;

  for (int u = 0; u <= n + 1; u++) {
    nd[u].adj = NIL;
  }

  for (int i = 1; i <= n; i++) {
    if (age[i] % 2) {
      add_edge(i, sink, 2);
    } else {
      add_edge(source, i, 2);
    }
  }

  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) {
      if ((age[j] % 2) && !composite[age[i] + age[j]]) {
        add_edge(i, j, 1);
      }
    }
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool bfs_reachable() {
  for (int u = 0; u <= n + 1; u++) {
    nd[u].parent = NIL;
  }
  q.clear();
  q.enqueue(source);

  while (!q.is_empty() && (nd[sink].parent == NIL)) {
    int u = q.dequeue();
    for (int ptr = nd[u].adj; ptr != NIL; ptr = e[ptr].next) {
      int v = e[ptr].v;
      if ((nd[v].parent == NIL) && e[ptr].cap) {
        nd[v].parent = u;
        nd[v].edge = ptr;
        q.enqueue(v);
      }
    }
  }

  return (nd[sink].parent != NIL);
}

int path_minimum() {
  int u = sink;
  int min_cap = e[nd[sink].edge].cap;

  while (u != source) {
    min_cap = min(min_cap, e[nd[u].edge].cap);
    u = nd[u].parent;
  }
  return min_cap;
}

void pump_on_path(int flow) {
  int u = sink;
  while (u != source) {
    e[nd[u].edge].cap -= flow;
    e[nd[u].edge ^ 1].cap += flow;
    u = nd[u].parent;
  }
}

int edmonds_karp() {
  int flow = 0;

  while (bfs_reachable()) {
    int augment = path_minimum();
    pump_on_path(augment);
    flow += augment;
  }

  return flow;
}

int find_neighbor(int u) {
  used[u] = true;

  for (int ptr = nd[u].adj; ptr != NIL; ptr = e[ptr].next) {
    int v = e[ptr].v;
    if (!used[v] &&
        (((age[u] % 2 == 0) && !e[ptr].cap) ||
         ((age[u] % 2 == 1) && e[ptr].cap))) {
      return v;
    }
  }

  return NIL;
}

void collect_cycle(int u) {
  int len = 0;

  cycle[num_cycles][len++] = u;
  while ((u = find_neighbor(u)) != NIL) {
    cycle[num_cycles][len++] = u;
  }

  cycle_len[num_cycles++] = len;
}

void collect_cycles() {
  used[source] = used[sink] = true;

  for (int i = 1; i <= n; i++) {
    if ((age[i] % 2 == 0) && !used[i]) {
      collect_cycle(i);
    }
  }
}

void write_cycles() {
  printf("%d\n", num_cycles);
  for (int i = 0; i < num_cycles; i++) {
    printf("%d", cycle_len[i]);
    for (int j = 0; j < cycle_len[i]; j++) {
      printf(" %d", cycle[i][j]);
    }
    printf("\n");
  }
}

int main() {
  read_data();

  if (possible) {
    sieve();
    build_graph();
    possible = (edmonds_karp() == n);
  }

  if (possible) {
    collect_cycles();
    write_cycles();
  } else {
    printf("Impossible\n");
  }

  return 0;
}
