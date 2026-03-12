#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 500'000;
const int MAX_EDGES = 1'000'000;
const int MAX_QUERIES = 500'000;
const int NONE = MAX_QUERIES + 1;

struct edge {
  int u, v, w;
  int q;
};

// Pădure de mulțimi disjuncte augmentată cu suport pentru undo. Reține
// modificările pe o stivă. Poate confirma modificările golind stiva.
struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int u_stack[MAX_NODES + 1], p_stack[MAX_NODES + 1];
  int stack_size;

  void init(int n) {
    stack_size = 0;
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  void save(int u) {
    u_stack[stack_size] = u;
    p_stack[stack_size] = p[u];
    stack_size++;
  }

  int find(int u) {
    if (p[u] == u) {
      return u;
    } else {
      save(u);
      p[u] = find(p[u]);
      return p[u];
    }
  }

  // Returnează true dacă și numai dacă u și v erau în arbori diferiți.
  bool unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (u == v) {
      return false;
    } else {
      save(u);
      p[u] = v;
      return true;
    }
  }

  void commit() {
    stack_size = 0;
  }

  void restore() {
    while (stack_size) {
      stack_size--;
      p[u_stack[stack_size]] = p_stack[stack_size];
    }
  }
};

edge e[MAX_EDGES];
disjoint_set_forest dsf;
bool answer[MAX_QUERIES];
int n, m, num_queries;

void read_data() {
  scanf("%d %d", &n, &m);
  for (int i = 0; i < m; i++) {
    scanf("%d %d %d", &e[i].u, &e[i].v, &e[i].w);
    e[i].q = NONE;
  }

  scanf("%d", &num_queries);
  for (int q = 0; q < num_queries; q++) {
    int qsize;
    scanf("%d", &qsize);
    while (qsize--) {
      int ind;
      scanf("%d", &ind);
      ind--;
      e[m++] = { e[ind].u, e[ind].v, e[ind].w, q };
    }
    answer[q] = true;
  }
}

void sort_edges() {
  std::sort(e, e + m, [] (edge& a, edge& b) {
    return (a.w < b.w) ||
      ((a.w == b.w) && (a.q < b.q));
  });
}

int find_batch(int start) {
  int end = start;
  while ((end < m) && (e[start].w == e[end].w) && (e[start].q == e[end].q)) {
    end++;
  }
  return end;
}

void process_equal_weights(int start, int end) {
  int k = start;
  while ((k < end) && answer[e[k].q]) {
    answer[e[k].q] = dsf.unite(e[k].u, e[k].v);
    k++;
  }
  dsf.restore();
}

void process_edges() {
  dsf.init(n);
  int start = 0;
  while (start < m) {
    if (e[start].q == NONE) {
      dsf.unite(e[start].u, e[start].v);
      dsf.commit();
      start++;
    } else {
      int end = find_batch(start);
      process_equal_weights(start, end);
      start = end;
    }
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%s\n", answer[i] ? "YES" : "NO");
  }
}

int main() {
  read_data();
  sort_edges();
  process_edges();
  write_answers();

  return 0;
}
