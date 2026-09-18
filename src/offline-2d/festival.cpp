#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_N = 100'000;
const int MAX_SEGTREE_NODES = 1 << 18;
const int INFINITY = 2'000'000'000;

struct festival {
  int x, y, sat; // timpul este y
};

struct normalizer {
  int v[MAX_N];
  int n;

  void add(int val) {
    v[n++] = val;
  }

  void sort() {
    std::sort(v, v + n);
    n = std::unique(v, v + n) - v;
  }

  int get_first_gte(int val) {
    return std::lower_bound(v, v + n, val) - v;
  }

  int get_last_lte(int val) {
    return std::upper_bound(v, v + n, val) - v - 1;
  }

  // Syntactic sugar pentru cînd val există garantat în v.
  int get(int val) {
    return get_first_gte(val);
  }
};

// Nodurile din AINT sînt AIB-uri.
struct segment_tree_node {
  std::vector<int> ys;
  std::vector<long long> values;

  void reserve_y(int y) {
    ys.push_back(y);
  }

  void build() {
    ys.push_back(-INFINITY); // pentru indexarea de la 1
    std::sort(ys.begin(), ys.end());
    auto last = std::unique(ys.begin(), ys.end());
    ys.erase(last, ys.end());
    values.resize(ys.size());
  }

  // Atenție, nu este garantat că y există în ys. Caută ultimul ys <= y.
  int find_y(int y) {
    return std::upper_bound(ys.begin(), ys.end(), y) - ys.begin() - 1;
  }

  void maximize_val(int y, long long val) {
    unsigned pos = find_y(y);
    do {
      values[pos] = std::max(values[pos], val);
      pos += pos & -pos;
    } while (pos < values.size());
  }

  long long prefix_max(int y) {
    unsigned pos = find_y(y);
    long long m = 0;
    while (pos) {
      m = std::max(m, values[pos]);
      pos &= pos - 1;
    }
    return m;
  }
};

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct segment_tree {
  segment_tree_node v[MAX_SEGTREE_NODES];
  int n;

  void init(int size) {
    n = next_power_of_2(size);
  }

  void reserve_point(int pos, int y) {
    for (pos += n; pos; pos /= 2) {
      v[pos].reserve_y(y);
    }
  }

  void build() {
    for (int pos = 1; pos < 2 * n; pos++) {
      v[pos].build();
    }
  }

  void maximize_val(int pos, int y, long long val) {
    for (pos += n; pos; pos /= 2) {
      v[pos].maximize_val(y, val);
    }
  }

  long long rectangle_max(int l, int r, int max_y) {
    long long result = 0;
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        result = std::max(result, v[l++].prefix_max(max_y));
      }
      l >>= 1;

      if (!(r & 1)) {
        result = std::max(result, v[r--].prefix_max(max_y));
      }
      r >>= 1;
    }

    return result;
  }
};

festival f[MAX_N];
normalizer norm;
segment_tree left_st, right_st;
long long answer;
int n, d;

void read_data() {
  FILE* fin = fopen("festival.in", "r");
  fscanf(fin, "%d %d", &n, &d);
  for (int i = 0; i < n; i++) {
    fscanf(fin, "%d %d %d", &f[i].y, &f[i].x, &f[i].sat);
  }
  fclose(fin);
}

void normalize_x() {
  for (int i = 0; i < n; i++) {
    norm.add(f[i].x);
  }
  norm.sort();
}

void sort_festivals_by_y() {
  std::sort(f, f + n, [](festival a, festival b) {
    return a.y < b.y;
  });
}

void dry_run() {
  left_st.init(norm.n);
  right_st.init(norm.n);

  for (int i = 0; i < n; i++) {
    int nx = norm.get(f[i].x);
    left_st.reserve_point(nx, f[i].y - f[i].x);
    right_st.reserve_point(nx, f[i].y + f[i].x);
  }

  left_st.build();
  right_st.build();
}

void process_festivals() {
  for (int i = 0; i < n; i++) {
    int nx = norm.get(f[i].x);
    int l_d = norm.get_first_gte(f[i].x - d);
    int r_d = norm.get_last_lte(f[i].x + d);
    long long l_score = left_st.rectangle_max(l_d, nx, f[i].y - f[i].x);
    long long r_score = right_st.rectangle_max(nx, r_d, f[i].y + f[i].x);
    long long score = f[i].sat + std::max(l_score, r_score);
    answer = std::max(answer, score);
    left_st.maximize_val(nx, f[i].y - f[i].x, score);
    right_st.maximize_val(nx, f[i].y + f[i].x, score);
  }
}

void write_answer() {
  FILE* f = fopen("festival.out", "w");
  fprintf(f, "%lld\n", answer);
}

int main() {
  read_data();
  normalize_x();
  sort_festivals_by_y();
  dry_run();
  process_festivals();
  write_answer();

  return 0;
}
