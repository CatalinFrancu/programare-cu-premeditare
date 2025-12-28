#include <algorithm>
#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_PATHS = 100'000;

struct cell {
  int v, next;
};

// Stochează două liste de noduri în fiecare nod u: una pentru căile care
// încep în u și alta pentru căile care se termină în u.
struct node {
  int adj;
  int path_begin, path_end; // pointers to list of nodes
  int start, finish;
};

struct fenwick_tree {
  unsigned short v[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int count(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int range_count(int x, int y) {
    return count(y) - count(x - 1);
  }
};

cell list[2 * MAX_NODES + 2 * MAX_PATHS + 1];
node nd[MAX_NODES + 1];
fenwick_tree active_start, active_finish;
int n, num_paths;
FILE* fin;

void add_to_list(int& head, int u) {
  static int ptr = 1;
  list[ptr] = { u, head };
  head = ptr++;
}

void read_tree() {
  fscanf(fin, "%d %d", &n, &num_paths);

  for (int i = 1; i < n; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    add_to_list(nd[u].adj, v);
    add_to_list(nd[v].adj, u);
  }
}

void read_paths() {
  for (int i = 0; i < num_paths; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    if (nd[u].start > nd[v].start) {
      int tmp = u; u = v; v = tmp;
    }
    add_to_list(nd[u].path_begin, v);
    add_to_list(nd[v].path_end, u);
  }
}

void compute_ranges(int u) {
  static int time = 0;

  nd[u].start = ++time;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].start) {
      compute_ranges(v);
    }
  }

  nd[u].finish = time;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int process_paths_starting_at(int u) {
  int cnt = 0;

  for (int ptr = nd[u].path_begin; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    active_finish.add(nd[v].start, +1);
    cnt++;
  }

  active_start.add(nd[u].start, +cnt);
  return cnt;
}

void process_paths_ending_at(int v) {
  int cnt = 0;

  for (int ptr = nd[v].path_end; ptr; ptr = list[ptr].next) {
    int u = list[ptr].v;
    active_start.add(nd[u].start, -1);
    cnt++;
  }

  active_finish.add(nd[v].start, -cnt);
}

int max_load = 0;

void compute_load(int u) {
  // Căi care se termină în subarborele lui u.
  int load = active_finish.range_count(nd[u].start, nd[u].finish);

  // Căi care încep în u.
  load += process_paths_starting_at(u);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (nd[v].start > nd[u].start) {
      compute_load(v);
      // Căi care au început într-un descendent al lui u și se vor termina fie
      // în alt fiu al lui u, fie în afara subarborelui lui u.
      load += active_start.range_count(nd[v].start, nd[v].finish);
    }
  }

  process_paths_ending_at(u);
  max_load = max(max_load, load);
}

void write_answer() {
  FILE* f = fopen("maxflow.out", "w");
  fprintf(f, "%d\n", max_load);
  fclose(f);
}

int main() {
  fin = fopen("maxflow.in", "r");
  read_tree();
  compute_ranges(1);
  read_paths();
  fclose(fin);

  active_start.init(n);
  active_finish.init(n);

  compute_load(1);
  write_answer();

  return 0;
}
