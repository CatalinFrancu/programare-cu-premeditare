#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_EDGES = 250'000;
const int INFINITY = 100'000'000;

struct node {
  int adj;
  int d;
  unsigned short relax_count;
};

struct cell {
  unsigned short v;
  short cost;
  int next;
};

struct queue {
  unsigned short v[MAX_NODES + 1];
  bool in_queue[MAX_NODES + 1];
  int head, tail;

  bool is_empty() {
    return head == tail;
  }

  void push(int u) {
    if (!in_queue[u]) {
      in_queue[u] = true;
      v[tail++] = u;
      if (tail == MAX_NODES + 1) {
        tail = 0;
      }
    }
  }

  int pop() {
    int u = v[head++];
    in_queue[u] = false;
    if (head == MAX_NODES + 1) {
      head = 0;
    }
    return u;
  }
};

cell list[MAX_EDGES];
node nd[MAX_NODES + 1];
queue q;
int n;
bool negative_cycle;

void add_edge(unsigned short u, unsigned short v, short cost) {
  static int pos = 1;

  list[pos] = { v, cost, nd[u].adj };
  nd[u].adj = pos++;
}

void read_data() {
  int num_edges;
  FILE* f = fopen("bellmanford.in", "r");
  fscanf(f, "%d %d", &n, &num_edges);

  while (num_edges--) {
    int u, v, cost;
    fscanf(f, "%d %d %d", &u, &v, &cost);
    add_edge(u, v, cost);
  }
  fclose(f);
}

void relax_all_edges(int u) {
  for (int pos = nd[u].adj; pos; pos = list[pos].next) {
    int v = list[pos].v, cost = list[pos].cost;

    if (nd[u].d + cost < nd[v].d) {
      nd[v].d = nd[u].d + cost;
      q.push(v);
    }
  }

  nd[u].relax_count++;
  negative_cycle |= (nd[u].relax_count == n);
}

void bellman_ford() {
  for (int u = 1; u <= n; u++) {
    nd[u].d = INFINITY;
  }
  nd[1].d = 0;
  q.push(1);

  while (!q.is_empty() && !negative_cycle) {
    int u = q.pop();
    relax_all_edges(u);
  }
}

void write_answer() {
  FILE* f = fopen("bellmanford.out", "w");
  if (negative_cycle) {
    fprintf(f, "Ciclu negativ!\n");
  } else {
    for (int u = 2; u <= n; u++) {
      fprintf(f, "%d ", nd[u].d);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_data();
  bellman_ford();
  write_answer();

  return 0;
}
