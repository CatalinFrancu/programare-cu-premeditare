#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 200'000;
const int MAX_QUERIES = 200'000;
const int T_EDGE = 0;
const int T_QUERY = 1;

struct disjoint_set_forest {
  int p[MAX_NODES + 1];
  int size[MAX_NODES + 1];

  void init(int n) {
    for (int u = 0; u < n; u++) {
      p[u] = u;
      size[u] = 1;
    }
  }

  int get_size(int u) {
    return size[find(u)];
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  void unite(int u, int v) {
    u = find(u);
    v = find(v);
    if (size[u] > size[v]) {
      std::swap(u, v);
    }
    p[u] = v;
    size[v] += size[u];
  }
};

struct event {
  int type;
  int weight;
  int u, v; // doar pentru muchii
  int id;   // doar pentru interogări
};

event evt[MAX_NODES + MAX_QUERIES];
disjoint_set_forest dsf;
long long answer[MAX_QUERIES];
int n, q, ne;

void read_data() {
  scanf("%d %d", &n, &q);
  for (int i = 0; i < n - 1; i++) {
    scanf("%d %d %d", &evt[ne].u, &evt[ne].v, &evt[ne].weight);
    evt[ne++].type = T_EDGE;
  }
  for (int i = 0; i < q; i++) {
    scanf("%d", &evt[ne].weight);
    evt[ne].type = T_QUERY;
    evt[ne++].id = i;
  }
}

void sort_events() {
  std::sort(evt, evt + ne, [](event& a, event& b) {
    return (a.weight < b.weight) ||
      ((a.weight == b.weight) && (a.type < b.type));
  });
}

void scan_events() {
  dsf.init(n);

  long long paths = 0;
  for (int i = 0; i < ne; i++) {
    if (evt[i].type == T_EDGE) {
      int u = evt[i].u, v = evt[i].v; // syntactic sugar
      paths += (long long)dsf.get_size(u) * dsf.get_size(v);
      dsf.unite(u, v);
    } else {
      answer[evt[i].id] = paths;
    }
  }
}

void write_answers() {
  for (int i = 0; i < q; i++) {
    printf("%lld ", answer[i]);
  }
  printf("\n");
}

int main() {
  read_data();
  sort_events();
  scan_events();
  write_answers();

  return 0;
}
