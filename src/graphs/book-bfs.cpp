#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_EDGES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int in_degree;
  int pass;
};

struct queue {
  int a[MAX_NODES];
  int head, tail;

  void init() {
    head = tail = 0;
  }

  void push(int u) {
    a[tail++] = u;
  }

  int pop() {
    return a[head++];
  }

  bool is_empty() {
    return head == tail;
  }
};

cell list[MAX_EDGES + 1];
node nd[MAX_NODES + 1];
queue q;
int n, num_edges, num_learned;

void clear_graph() {
  num_edges = 0;
  num_learned = 0;
  for (int u = 1; u <= n; u++) {
    nd[u].adj = 0;
    nd[u].pass = 0;
  }
}

void add_edge(int u, int v) {
  list[++num_edges] = { v, nd[u].adj };
  nd[u].adj = num_edges;
}

void read_graph() {
  scanf("%d", &n);

  clear_graph();
  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].in_degree);
    for (int i = 0; i < nd[u].in_degree; i++) {
      int v;
      scanf("%d", &v);
      add_edge(v, u);
    }
  }
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

void update_passes(int u, int v) {
  int pass_v = (u < v) ? nd[u].pass : (nd[u].pass + 1);
  nd[v].pass = max(nd[v].pass, pass_v);
}

void bfs() {
  q.init();

  for (int u = 1; u <= n; u++) {
    if (!nd[u].in_degree) {
      nd[u].pass = 1;
      q.push(u);
    }
  }

  while (!q.is_empty()) {
    num_learned++;
    int u = q.pop();
    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      update_passes(u, v);
      nd[v].in_degree--;
      if (!nd[v].in_degree) {
        q.push(v);
      }
    }
  }
}

int get_max_passes() {
  if (num_learned < n) {
    return -1;
  }

  int m = 0;
  for (int u = 1; u <= n; u++) {
    m = max(m, nd[u].pass);
  }
  return m;
}

void solve_test() {
  read_graph();
  bfs();
  int answer = get_max_passes();
  printf("%d\n", answer);
}

int main() {

  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
