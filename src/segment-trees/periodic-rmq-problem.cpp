#include <algorithm>
#include <stdio.h>

const int MAX_N = 100'000;
const int MAX_SPARSE_TABLE_LAYERS = 17;

const int MAX_OPS = 128 * 1024;
const int MAX_ENDPOINTS = 2 * MAX_OPS;
const int MAX_COMPRESSED_RANGES = 2 * MAX_ENDPOINTS;
const int T_UPDATE = 1;
const int T_QUERY = 2;

const int INFINITY = 2'000'000'000;

struct operation {
  int type;
  int l, r;
  int val;
};

int min(int x, int y) {
  return (x < y) ? x : y;
}

int next_power_of_2(int n) {
  return 1 << (32 - __builtin_clz(n - 1));
}

int log2(int n) {
  return 31 - __builtin_clz(n);
}

struct base_array {
  int v[MAX_SPARSE_TABLE_LAYERS][MAX_N];
  int n;

  void build_rmq() {
    for (int i = 1, size = 2; size <= n; i++, size *= 2) {
      for (int j = 0; j + size <= n; j++) {
        v[i][j] = min(v[i - 1][j], v[i - 1][j + size / 2]);
      }
    }
  }

  int rmq(int l, int r) {
    int row = log2(r - l + 1); // De ex. rîndul 3 pentru lungimile 8-15.
    return min(v[row][l],
               v[row][r - (1 << row) + 1]);
  }

  int periodic_rmq(int l, int r) {
    if (r - l + 1 >= n) {
      return rmq(0, n - 1);
    } else {
      l %= n;
      r %= n;
      if (l <= r) {
        return rmq(l, r);
      } else {
        return min(rmq(l, n - 1), rmq(0, r));
      }
    }
  }
};

// Un arbore de intervale cu atribuire pe interval și RMQ pe interval.
//
// Contract: m[x] este minimul nodurilor subîntinse de x. Cînd dirty[x] este
// true, atunci m[x] trebuie atribuit pe tot intervalul.
struct segment_tree {
  int m[2 * MAX_COMPRESSED_RANGES];
  bool dirty[2 * MAX_COMPRESSED_RANGES];
  int n;

  void init(int n) {
    this->n = next_power_of_2(n);
    for (int i = this->n; i < 2 * this->n; i++) {
      m[i] = INFINITY;
    }
  }

  void raw_set(int pos, int val) {
    m[pos + n] = val;
  }

  void build() {
    for (int i = n - 1; i; i--) {
      m[i] = min(m[2 * i], m[2 * i + 1]);
    }
  }

  void push(int x) {
    if (dirty[x]) {
      dirty[2 * x] = dirty[2 * x + 1] = true;
      m[2 * x] = m[2 * x + 1] = m[x];
      dirty[x] = false;
    }
  }

  void push_path(int x) {
    int bits = __builtin_popcount(n - 1);
    for (int b = bits; b; b--) {
      push(x >> b);
    }
  }

  void pull_path(int x) {
    for (x /= 2; x; x /= 2) {
      if (!dirty[x]) {
        m[x] = min(m[2 * x], m[2 * x + 1]);
      }
    }
  }

  void range_set(int l, int r, int val) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;

    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        m[l] = val;
        dirty[l++] = true;
      }
      l >>= 1;

      if (!(r & 1)) {
        m[r] = val;
        dirty[r--] = true;
      }
      r >>= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  int rmq(int l, int r) {
    int result = INFINITY;

    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        result = min(result, m[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        result = min(result, m[r--]);
      }
      r >>= 1;
    }

    return result;
  }

};

base_array base;
operation op[MAX_OPS];
int coord[MAX_COMPRESSED_RANGES];
segment_tree st;
int num_ops, num_ranges;

void read_data() {
  int ignored;
  scanf("%d %d", &base.n, &ignored);
  for (int i = 0; i < base.n; i++) {
    scanf("%d", &base.v[0][i]);
  }
  scanf("%d", &num_ops);
  for (int i = 0; i < num_ops; i++) {
    scanf("%d %d %d", &op[i].type, &op[i].l, &op[i].r);
    op[i].l--;
    op[i].r--;
    if (op[i].type == T_UPDATE) {
      scanf("%d", &op[i].val);
    }
  }
}

void remove_duplicates() {
  int size = 1;
  for (int i = 1; i < num_ranges; i++) {
    if (coord[i] != coord[i - 1]) {
      coord[size++] = coord[i];
    }
  }
  num_ranges = size;
}

int bin_search(int val) {
  int l = 0, r = num_ranges;
  while (coord[l] != val) {
    int mid = (l + r) / 2;
    if (coord[mid] > val) {
      r = mid;
    } else {
      l = mid;
    }
  }
  return l;
}

void normalize_endpoints() {
  for (int i = 0; i < num_ops; i++) {
    coord[num_ranges++] = op[i].l;
    coord[num_ranges++] = op[i].l + 1;
    coord[num_ranges++] = op[i].r;
    coord[num_ranges++] = op[i].r + 1;
  }

  std::sort(coord, coord + num_ranges);
  remove_duplicates();

  for (int i = 0; i < num_ops; i++) {
    op[i].l = bin_search(op[i].l);
    op[i].r = bin_search(op[i].r);
  }
}

void build_compressed_segtree() {
  st.init(num_ranges - 1);

  for (int i = 0; i < num_ranges - 1; i++) {
    int rmq = base.periodic_rmq(coord[i], coord[i + 1] - 1);
    st.raw_set(i, rmq);
  }

  st.build();
}

void process_ops() {
  for (int i = 0; i < num_ops; i++) {
    if (op[i].type == T_UPDATE) {
      st.range_set(op[i].l, op[i].r, op[i].val);
    } else {
      int m = st.rmq(op[i].l, op[i].r);
      printf("%d\n", m);
    }
  }
}

int main() {
  read_data();
  normalize_endpoints();
  base.build_rmq();
  build_compressed_segtree();
  process_ops();

  return 0;
}
