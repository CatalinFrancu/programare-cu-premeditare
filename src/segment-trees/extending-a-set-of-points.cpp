#include <stdio.h>
#include <unordered_map>
#include <vector>

const int MAX_OPS = 300'000;
const int MAX_COORD = 300'000;
const int MAX_SEGTREE_NODES = 1 << 20;

struct point {
  int x, y;

  bool operator==(const point &other) const {
    return (x == other.x) && (y == other.y);
  }
};

struct point_hash {
  std::size_t operator()(const point& p) const {
    return p.x * MAX_COORD + p.y;
  }
};

struct change {
  int u, v;
  int par_u;
  int size_x_v, size_y_v;
  long long total_area;
};

struct disjoint_set_forest {
  int par[2 * MAX_COORD + 1];
  int size_x[2 * MAX_COORD + 1], size_y[2 * MAX_COORD + 1];
  long long total_area;

  // Stiva de modificări făcute.
  change st[MAX_COORD];
  int ss;

  void init() {
    // Valorile inițiale pentru size_x și size_y elimină cazurile particulare.
    // Acum formula reuniunii (k·l, k'·l') ⇒ (k+k')·(l+l') este valabilă și
    // prima dată cînd întîlnim un anume x sau y.
    for (int i = 0; i < MAX_COORD; i++) {
      par[i] = i;
      size_x[i] = 0;
      size_y[i] = 1;
    }
    for (int i = 0; i < MAX_COORD; i++) {
      par[i + MAX_COORD] = i + MAX_COORD;
      size_x[i + MAX_COORD] = 1;
      size_y[i + MAX_COORD] = 0;
    }
    total_area = 0;
  }

  void push_change(int u, int v) {
    st[ss++] = {
      .u = u,
      .v = v,
      .par_u = par[u],
      .size_x_v = size_x[v],
      .size_y_v = size_y[v],
      .total_area = total_area,
    };
  }

  int find(int u) {
    return (par[u] == u) ? u : find(par[u]);
  }

  void unite(int u, int v) {
    if (u != v) {
      par[u] = v;
      total_area -= (long long)size_x[u] * size_y[u];
      total_area -= (long long)size_x[v] * size_y[v];
      size_x[v] += size_x[u];
      size_y[v] += size_y[u];
      total_area += (long long)size_x[v] * size_y[v];
    }
  }

  void process_point(point p) {
    int u = find(p.x);
    int v = find(p.y + MAX_COORD);
    if ((long long)size_x[u] * size_y[u] >
        (long long)size_x[v] * size_y[v]) {
      int tmp = u; u = v; v = tmp;
    }

    push_change(u, v);
    unite(u, v);
  }

  void rollback() {
    change& c = st[--ss];
    par[c.u] = c.par_u;
    size_x[c.v] = c.size_x_v;
    size_y[c.v] = c.size_y_v;
    total_area = c.total_area;
  }
};

long long sol[MAX_OPS];

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct segment_tree {
  std::vector<point> points[MAX_SEGTREE_NODES];
  disjoint_set_forest grids;
  int n, orig_n;

  void init(int _n) {
    orig_n = _n;
    n = next_power_of_2(_n);
    grids.init();
  }

  void add_point_on_range(point p, int l, int r) {
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        points[l++].push_back(p);
      }
      l >>= 1;

      if (!(r & 1)) {
        points[r--].push_back(p);
      }
      r >>= 1;
    }
  }

  void dfs(int node) {
    for (point p: points[node]) {
      grids.process_point(p);
    }

    if (node >= n) {
      if (node - n < orig_n) {
        sol[node - n] = grids.total_area;
      }
    } else {
      dfs(2 * node);
      dfs(2 * node + 1);
    }

    for (unsigned i = 0; i < points[node].size(); i++) {
      grids.rollback();
    }
  }
};

segment_tree st;
int n;

void read_intervals_into_segtree() {
  scanf("%d", &n);
  st.init(n);
  std::unordered_map<point, int, point_hash> map;

  for (int time = 0; time < n; time++) {
    point p;
    scanf("%d %d", &p.x, &p.y);
    p.x--;
    p.y--;
    auto it = map.find(p);
    if (it == map.end()) {
      map[p] = time;
    } else {
      st.add_point_on_range(p, it->second, time - 1);
      map.erase(p);
    }
  }

  for (auto key_val: map) {
    st.add_point_on_range(key_val.first, key_val.second, n - 1);
  }
}

void write_answers() {
  for (int i = 0; i < n; i++) {
    printf("%lld%c", sol[i], (i == n - 1) ? '\n' : ' ');
  }
}

int main() {
  read_intervals_into_segtree();
  st.dfs(1);
  write_answers();

  return 0;
}
