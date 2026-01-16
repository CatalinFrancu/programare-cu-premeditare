#include <stdio.h>

const int MAX_NODES = 100'000;
const int BLACK = 0;
const int WHITE = 1;
const int T_QUERY = 0;
const int T_TOGGLE = 1;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;  // fiul cu cel mai mare subarbore
  int head;   // începutul lanțului nostru
  int pos;    // timpul de descoperire în DFS / poziția în liniarizare
  int size;   // mărimea subarborelui (indiferent de culoare)
  bool color;
};

// Un AIB cu adăugare pe interval și interogare punctuală.
struct range_add_fenwick_tree {
  int v[MAX_NODES + 2];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int delta) {
    do {
      v[pos] += delta;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void range_add(int l, int r, int delta) {
    add(l, delta);
    add(r + 1, -delta);
  }

  void point_add(int pos, int delta) {
    range_add(pos, pos, delta);
  }

  int point_query(int pos) {
    int sum = 0;
    while (pos) {
      sum += v[pos];
      pos &= pos - 1;
    }
    return sum;
  }
};

// Un AIB de booleeni cu suport pentru căutări binare.
struct bool_fenwick_tree {
  int v[MAX_NODES + 1];
  int n;
  int max_p2;

  void init(int n, bool val) {
    this->n = n;
    max_p2 = 1 << (31 - __builtin_clz(n));

    for (int i = 1; i <= n; i++) {
      v[i] += val;
      int j = i + (i & -i);
      if (j <= n) {
        v[j] += v[i];
      }
    }
  }

  void add(int pos, int delta) {
    do {
      v[pos] += delta;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void set(int pos) {
    add(pos, +1);
  }

  void clear(int pos) {
    add(pos, -1);
  }

  int prefix_sum(int pos) {
    int sum = 0;
    while (pos) {
      sum += v[pos];
      pos &= pos - 1;
    }
    return sum;
  }

  int last_occurrence_of_partial_sum(int sum) {
    int pos = 0;

    for (int interval = max_p2; interval; interval >>= 1) {
      if ((pos + interval <= n) && (v[pos + interval] < sum)) {
        sum -= v[pos + interval];
        pos += interval;
      }
    }

    return pos;
  }

  // Returnează poziția ultimului 1 pe o poziție ≤ pos.
  int last_one_before(int pos) {
    int s = prefix_sum(pos);
    if (s) {
      return 1 + last_occurrence_of_partial_sum(s);
    } else {
      return 0;
    }
  }
};

cell list[2 * MAX_NODES];
range_add_fenwick_tree size[2];
bool_fenwick_tree color[2];
node nd[MAX_NODES + 1];
int hld_order[MAX_NODES + 1];
int n;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d", &n);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

void init_fenwick_trees() {
  size[BLACK].init(n);
  size[WHITE].init(n);
  color[BLACK].init(n, 1);
  color[WHITE].init(n, 0);
}

void heavy_dfs(int u) {
  nd[u].size = 1;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      heavy_dfs(v);
      nd[u].size += nd[v].size;
      if (!nd[u].heavy || (nd[v].size > nd[nd[u].heavy].size)) {
        nd[u].heavy = v;
      }

    }
  }
}

void decompose_dfs(int u, int head) {
  static int time = 1;

  nd[u].head = head;
  size[BLACK].point_add(time, nd[u].size);
  size[WHITE].point_add(time, 1);
  hld_order[time] = u;
  nd[u].pos = time++;

  if (nd[u].heavy) {
    decompose_dfs(nd[u].heavy, head);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && v != nd[u].heavy) {
      decompose_dfs(v, v); // începe un lanț nou
    }
  }
}

// Returnează cel mai depărtat strămoș w pe același lanț greu cu u astfel
// încît (1) toate nodurile de la w la u inclusiv au aceeași culoare și (2)
// părintele lui w are o culoare diferită. Notă: părintele lui w poate fi pe
// lanțul următor. Returnează 0 dacă w nu există.
int get_top_same_color(int u) {
  int start = nd[nd[u].head].pos;
  int pos_1 = color[!nd[u].color].last_one_before(nd[u].pos);
  if (pos_1 >= start) {
    // Există un nod de culoare opusă pe lanț.
    return hld_order[pos_1 + 1];
  }

  int next_path = nd[nd[u].head].parent;
  if (nd[next_path].color != nd[u].color) {
    // Părintele capului de lanț are culoarea opusă.
    return nd[u].head;
  }

  return 0;
}

int query(int u) {
  nd[0].color = !nd[u].color; // santinelă

  int t;
  while (u && (t = get_top_same_color(u)) == 0) {
    u = nd[nd[u].head].parent;
  }

  return size[nd[t].color].point_query(nd[t].pos);
}

void path_update(int u, int delta1, int delta2) {
  int c = nd[u].color;
  size[!c].point_add(nd[u].pos, delta1);

  // Toți strămoșii cîștigă/pierd cîtă vreme au culoarea părintelui.
  int t;
  while (u && (t = get_top_same_color(u)) == 0) {
    size[c].range_add(nd[nd[u].head].pos, nd[u].pos, delta2);
    u = nd[nd[u].head].parent;
  }

  if (u) {
    size[c].range_add(nd[t].pos, nd[u].pos, delta2);

    // Și primul strămoș de culoarea opusă cîștigă/pierde.
    t = nd[t].parent;
    if (t) {
      size[c].point_add(nd[t].pos, delta2);
    }
  }
}

void toggle(int u) {
  int old = nd[u].color;
  nd[u].color = !old;

  color[old].clear(nd[u].pos);
  color[!old].set(nd[u].pos);

  int p = nd[u].parent;
  if (p) {
    int lose = size[old].point_query(nd[u].pos);
    int gain = size[!old].point_query(nd[u].pos);
    if (nd[u].color == nd[p].color) {
      path_update(p, -lose, gain);
    } else {
      path_update(p, gain, -lose);
    }
  }
}

void process_ops() {
  int num_queries, type, u;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &type, &u);
    if (type == T_QUERY) {
      printf("%d\n", query(u));
    } else {
      toggle(u);
    }
  }
}

int main() {
  read_tree();
  init_fenwick_trees();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  process_ops();

  return 0;
}
