// Complexitate: O(n log n).
#include <stdio.h>
#include <stdlib.h>

typedef long long i64;

const int MAX_NODES = 200'000;
const int MAX_P2 = 1 << 18;
const int INF = 1'000'000'001;
const int NIL = -1;

const int TYPE_UPDATE = 1;

int min(int x, int y) {
  return (x < y) ? x : y;
}

struct treap_node {
  int key, pri;
  i64 sum;
  int cnt;
  int l, r;
};

treap_node treap_buf[2 * MAX_NODES];
int treap_ptr;

struct treap {
  treap_node *v;
  int n;

  treap_node make_node(int key, int pri) {
    return {
      .key = key,
      .pri = pri,
      .sum = key,
      .cnt = 1,
      .l = NIL,
      .r = NIL,
    };
  }

  void init(int size) {
    v = treap_buf + treap_ptr;
    treap_ptr += size + 1;
    v[0] = make_node(INF, INF);
    n = 1;
  }

  int get_count(int t) {
    return (t == NIL) ? 0 : v[t].cnt;
  }

  i64 get_sum(int t) {
    return (t == NIL) ? 0 : v[t].sum;
  }

  void update_sum_cnt(int t) {
    v[t].sum = get_sum(v[t].l) + get_sum(v[t].r) + v[t].key;
    v[t].cnt = get_count(v[t].l) + get_count(v[t].r) + 1;
  }

  void split(int t, int key, int& l, int& r) {
    if (t == NIL) {
      l = r = NIL;
    } else if (v[t].key <= key) {
      split(v[t].r, key, v[t].r, r);
      l = t;
      update_sum_cnt(t);
    } else {
      split(v[t].l, key, l, v[t].l);
      r = t;
      update_sum_cnt(t);
    }
  }

  void insert(int& t, int elem) {
    if (t == NIL) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].key, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(v[t].key <= v[elem].key ? v[t].r : v[t].l, elem);
    }
    update_sum_cnt(t);
  }

  void insert(int key) {
    v[n++] = make_node(key, rand());
    insert(v[0].l, n - 1);
  }

  void merge(int& t, int l, int r) {
    if (l == NIL) {
      t = r;
    } else if (r == NIL) {
      t = l;
    } else if (v[l].pri > v[r].pri) {
      merge(v[l].r, v[l].r, r);
      t = l;
    } else {
      merge(v[r].l, l, v[r].l);
      t = r;
    }
    if (t != NIL) {
      update_sum_cnt(t);
    }
  }

  // Returnează nodul șters.
  int erase(int& t, int key) {
    int result;
    if (v[t].key == key) {
      result = t;
      merge(t, v[t].l, v[t].r);
    } else {
      result = erase(key < v[t].key ? v[t].l : v[t].r, key);
    }
    if (t != NIL) {
      update_sum_cnt(t);
    }
    return result;
  }

  void change(int old_key, int new_key) {
    int t = erase(v[0].l, old_key);
    v[t] = make_node(new_key, rand());
    insert(v[0].l, t);
  }

  void totals_up_to(int key, int& cnt, i64& sum) {
    cnt = sum = 0;

    int t = 0;
    while (t != NIL) {
      if (key > v[t].key) {
        cnt += get_count(v[t].l) + 1;
        sum += get_sum(v[t].l) + v[t].key;
        t = v[t].r;
      } else {
        t = v[t].l;
      }
    }
  }

  // Returnează suma contribuțiilor pe muchiile spre fii cînd valoarea nodului
  // se schimbă.
  i64 get_contrib_change(int old_val, int new_val) {
    int min_val = min(old_val, new_val);
    int max_val = old_val + new_val - min_val;

    int l, r;
    i64 sum_l, sum_center;
    int size = n - 1;

    totals_up_to(min_val, l, sum_l);
    totals_up_to(max_val, r, sum_center);
    sum_center -= sum_l;

    i64 delta =
      sum_center                   // contribuții noi pe [l, r)
      + (i64)(size - r) * max_val  // contribuții noi pe [r, end)
      - (i64)(size - l) * min_val; // contribuții vechi pe [l, end)

    return (new_val > old_val) ? delta : -delta;
  }
};

struct node {
  int adj;
  int val;
  int parent;
  int num_children;
  int tin, tout;
  treap trp;
};

struct edge {
  int v, next;
};

struct segment_tree_node {
  i64 sum;     // suma valorilor nodurilor din subarbore
  i64 contrib; // suma contribuțiilor nodurilor din subarbore
  int min_val; // valoarea minimă a unui nod din subarbore

  void include(segment_tree_node& other) {
    sum += other.sum;
    contrib += other.contrib;
    min_val = min(min_val, other.min_val);
  }
};

struct segment_tree {
  segment_tree_node v[2 * MAX_P2];
  int n;

  void init(int n) {
    while (n & (n - 1)) {
      n += (n & -n);
    }
    this->n = n;
    for (int i = n; i <= 2 * n; i++) {
      v[i].min_val = INF;
    }
  }

  void set_value(int pos, int val) {
    pos += n;
    int delta = val - v[pos].sum;
    v[pos].sum = v[pos].min_val = val;

    for (pos >>= 1; pos; pos >>= 1) {
      v[pos].sum += delta;
      v[pos].min_val = min(v[2 * pos].min_val, v[2 * pos + 1].min_val);
    }
  }

  void add_contrib(int pos, i64 delta) {
    for (pos += n; pos; pos >>= 1) {
      v[pos].contrib += delta;
    }
  }

  // [l, r] interval închis
  i64 get_collapse_cost(int l, int r) {
    segment_tree_node acc = { 0, 0, INF };
    l += n;
    r += n;

    while (l <= r)  {
      if (l & 1) {
        acc.include(v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        acc.include(v[r--]);
      }
      r >>= 1;
    }

    return acc.sum - acc.min_val - acc.contrib;
  }
};

node a[MAX_NODES + 1];
edge e[MAX_NODES];
segment_tree st;
int n, tact;

void add_edge(int u, int parent) {
  static int pos = 1;
  a[u].parent = parent;
  a[parent].num_children++;
  e[pos] = { u, a[parent].adj };
  a[parent].adj = pos++;
}

void read_tree() {
  scanf("%d", &n);
  for (int u = 1; u <= n; u++) {
    scanf("%d", &a[u].val);
  }
  for (int u = 2; u <= n; u++) {
    int p;
    scanf("%d", &p);
    add_edge(u, p);
  }
};

void linearize_dfs(int u) {
  a[u].tin = tact++;

  for (int p = a[u].adj; p; p = e[p].next) {
    linearize_dfs(e[p].v);
  }

  a[u].tout = tact - 1; // Vrem ca [tin, tout] să fie închis.
}

void build_segment_tree() {
  st.init(n + 1);

  for (int u = 1; u <= n; u++) {
    st.set_value(a[u].tin, a[u].val);

    i64 contrib = 0;
    a[u].trp.init(a[u].num_children);
    for (int p = a[u].adj; p; p = e[p].next) {
      int v = e[p].v;
      a[u].trp.insert(a[v].val);
      contrib += min(a[u].val, a[v].val);
    }
    st.add_contrib(a[u].tin, contrib);
  }
}

void update(int u, int val) {
  // modificarea contribuției în u
  i64 delta = a[u].trp.get_contrib_change(a[u].val, val);
  st.add_contrib(a[u].tin, delta);

  // modificarea contribuției în părintele lui u
  int p = a[u].parent;
  if (p) {
    a[p].trp.change(a[u].val, val);
    delta = min(a[p].val, val) - min(a[p].val, a[u].val);
    st.add_contrib(a[p].tin, delta);
  }

  // modificarea valorii
  a[u].val = val;
  st.set_value(a[u].tin, a[u].val);
}

i64 query(int u) {
  return st.get_collapse_cost(a[u].tin, a[u].tout);
}

void process_ops() {
  int num_ops, type, u, val;

  scanf("%d", &num_ops);
  while (num_ops--) {
    scanf("%d %d", &type, &u);
    if (type == TYPE_UPDATE) {
      scanf("%d", &val);
      update(u, val);
    } else {
      printf("%lld\n", query(u));
    }
  }
}

int main() {
  read_tree();
  linearize_dfs(1);
  build_segment_tree();
  process_ops();

  return 0;
}
