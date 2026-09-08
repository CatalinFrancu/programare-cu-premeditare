#include <stdio.h>

const int MAX_NODES = 100'000;
const int MOD = 1'000'000'007;

struct stats {
  int cnt, sum;

  stats operator -(stats other) {
    return { cnt - other.cnt, (sum + MOD - other.sum) % MOD };
  }

  stats operator +=(stats other) {
    cnt += other.cnt;
    sum = (sum + other.sum) % MOD;
    return *this;
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int size;
  int val;
  bool dead;
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
// prev[d] = numărul și suma tuturor căilor centroid-u pentru toate nodurile u
// de adîncime d din fiii văzuți anterior
stats prev[MAX_NODES], sum_prev;
int p;
int answer;

void add_child(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  int n;
  scanf("%d %d", &n, &p);

  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].val);
  }

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_child(u, v);
    add_child(v, u);
  }
}

// Combină calea curentă centroid-u, de greutate sum_r, cu statisticile l
// despre căile de adîncime cel mult p - depth din fiii anteriori.
void count_paths(int u, int parent, int depth, stats l, int sum_r) {
  sum_r = (sum_r + nd[u].val) % MOD;
  answer = (answer + l.sum + (long long)l.cnt * sum_r) % MOD;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((!nd[v].dead) && (v != parent) && (depth < p)) {
      count_paths(v, u, depth + 1, l - prev[p - depth], sum_r);
    }
  }
}

// Adaugă calea curentă centroid-u, de greutate sum, la statistici.
void add_paths(int u, int parent, int depth, int sum) {
  sum = (sum + nd[u].val) % MOD;
  if (depth < p) {
    prev[depth] += {1, sum};
    sum_prev += {1, sum};
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((!nd[v].dead) && (v != parent) && (depth < p)) {
      add_paths(v, u, depth + 1, sum);
    }
  }
}

void clear_prev(int depth) {
  for (int d = 0; (d < depth) && (d < p); d++) {
    prev[d] = {0, 0};
  }
  sum_prev = {0, 0};
}

void process_paths_through(int u) {
  prev[0] = sum_prev = {1, nd[u].val};

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].dead) {
      count_paths(v, u, 1, sum_prev, 0);
      add_paths(v, u, 1, nd[u].val);
    }
  }

  clear_prev(nd[u].size);
}

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((v != parent) && !nd[v].dead) {
      size_dfs(v, u);
      nd[u].size += nd[v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if ((nd[v].size < nd[u].size) && (nd[v].size > limit)) {
      return find_centroid(v, limit);
    }
  }

  return u;
}

void decompose(int u) {
  size_dfs(u, 0);
  u = find_centroid(u, nd[u].size / 2);

  process_paths_through(u);

  nd[u].dead = true;
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].dead) {
      decompose(v);
    }
  }
}

int main() {
  read_input_data();
  decompose(1);
  printf("%d\n", answer);

  return 0;
}
