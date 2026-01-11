#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_LENGTH = 10;
const int SENTINEL = 100'001; // populația maximă + 1

int min(int x, int y) {
  return (x < y) ? x : y;
}

int rbuf[2 * MAX_LENGTH]; // pentru combinarea a două liste

// Un *roster* este o listă ordonată de ID-uri de persoane.
struct roster {
  int n;
  int v[MAX_LENGTH + 1];

  void clear() {
    n = 0;
  }

  void add(int id) {
    int k = n;
    while (k && (id < v[k - 1])) {
      v[k] = v[k - 1];
      k--;
    }
    v[k] = id;
    if (n < MAX_LENGTH) {
      n++;
    }
  }

  void copy_from(roster& other) {
    n = other.n;
    for (int i = 0; i <= n; i++) {
      v[i] = other.v[i];
    }
  }

  void merge_from(roster& other, int limit) {
    v[n] = other.v[other.n] = SENTINEL;
    n = min(n + other.n, limit);
    int i = 0, j = 0;

    for (int k = 0; k < n; k++) {
      if (v[i] < other.v[j]) {
        rbuf[k] = v[i++];
      } else {
        rbuf[k] = other.v[j++];
      }
    }

    for (int i = 0; i < n; i++) {
      v[i] = rbuf[i];
    }
  }

  void print() {
    printf("%d", n);
    for (int i = 0; i < n; i++) {
      printf(" %d", v[i]);
    }
    printf("\n");
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int depth;
  int time_in, time_out;
  int parent;
  int jump;

  // populația proprie
  roster ids;

  // populația pînă la destinația saltului, exclusivdestination
  roster jids;

  void compute_jump();
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int num_queries, num_people;

void node::compute_jump() {
  jids.copy_from(ids);

  int u = parent, u2 = nd[u].jump, u3 = nd[u2].jump;
  if (u3 && (nd[u2].depth - nd[u].depth == nd[u3].depth - nd[u2].depth)) {
    jump = u3;
    jids.merge_from(nd[u].jids, MAX_LENGTH);
    jids.merge_from(nd[u2].jids, MAX_LENGTH);
  } else {
    jump = u;
  }
}

void add_edge(int u, int v) {
  static int pos = 1;
  list[pos] = { v, nd[u].adj };
  nd[u].adj = pos++;
}

void read_input_data() {
  int n, u, v;
  scanf("%d %d %d", &n, &num_people, &num_queries);

  for (int i = 0; i < n - 1; i++) {
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }

  for (int id = 1; id <= num_people; id++) {
    scanf("%d", &u);
    nd[u].ids.add(id);
  }
}

// Traversează arborele și calculează părinții, adîncimile, *jump pointers* și
// listele subîntinse de toți pointerii.
void dfs(int u) {
  static int time = 1;
  nd[u].time_in = time++;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].depth) {
      nd[v].depth = 1 + nd[u].depth;
      nd[v].parent = u;
      nd[v].compute_jump();
      dfs(v);
    }
  }

  nd[u].time_out = time++;
}

bool is_ancestor(int u, int v) {
  return
    (nd[u].time_in <= nd[v].time_in) &&
    (nd[u].time_out >= nd[v].time_out);
}

int climb_until_ancestor(int u, int v, int limit, roster& dest) {
  while (!is_ancestor(u, v)) {
    if (nd[u].jump && !is_ancestor(nd[u].jump, v)) {
      dest.merge_from(nd[u].jids, limit);
      u = nd[u].jump;
    } else {
      dest.merge_from(nd[u].ids, limit);
      u = nd[u].parent;
    }
  }

  return u;
}

// Urcă cu u și v pînă la LCA și colectează listele de pe drumuri.
void two_ptr_lca(int u, int v, int limit, roster& dest) {
  dest.clear();
  u = climb_until_ancestor(u, v, limit, dest);
  v = climb_until_ancestor(v, u, limit, dest);
  dest.merge_from(nd[u].ids, limit);
}

void answer_queries() {
  int u, v, limit;
  roster r;
  while (num_queries--) {
    scanf("%d %d %d", &u, &v, &limit);
    two_ptr_lca(u, v, limit, r);
    r.print();
  }
}

int main() {
  read_input_data();
  nd[1].depth = 1;
  dfs(1);
  answer_queries();

  return 0;
}
