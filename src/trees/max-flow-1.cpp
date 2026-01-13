#include <stdio.h>

const int MAX_NODES = 50'000;
const int MAX_PATHS = 100'000;
const int NIL = -1;

struct list {
  int val, next;
};

struct node {
  int parent;
  int ptr; // pointer în lista de adiacență
  int path_ptr; // pointer în lista de căi
  int load;
};

struct path {
  int u, v, lca;
};

list adj[2 * MAX_NODES];
list path_list[2 * MAX_PATHS];
node nd[MAX_NODES + 1];
path p[MAX_PATHS];
int ds_parent[MAX_NODES + 1];
int n, num_paths;

/*************************************************************************/
/*                  Implementare de mulțimi disjuncte.                   */
/*************************************************************************/

void ds_init() {
  for (int u = 1; u <= n; u++) {
    ds_parent[u] = u;
  }
}

int ds_find(int u) {
  return (ds_parent[u] == u)
    ? u
    : (ds_parent[u] = ds_find(ds_parent[u]));
}

// Întotdeauna unifică v în u. Fără unificare după rang.
void ds_union(int u, int v) {
  ds_parent[ds_find(v)] = ds_find(u);
}

/*************************************************************************/
/*      Implementarea algoritmului lui Tarjan pentru LCA offline.        */
/*************************************************************************/

void offline_lca_dfs(int u, int parent) {
  nd[u].parent = parent;

  for (int ptr = nd[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (v != parent) {
      offline_lca_dfs(v, u);
      ds_union(u, v);
    }
  }

  for (int ptr = nd[u].path_ptr; ptr != NIL; ptr = path_list[ptr].next) {
    int i = path_list[ptr].val;
    int v = (p[i].u == u) ? p[i].v : p[i].u;
    if (nd[v].parent) {
      p[i].lca = ds_find(v);
    }
  }
}

void offline_lca() {
  ds_init();
  offline_lca_dfs(1, 1);
}

/*************************************************************************/
/*                           Codul principal.                            */
/*************************************************************************/

void add_neighbor(int u, int v) {
  static int ptr = 0;
  adj[ptr] = { v, nd[u].ptr };
  nd[u].ptr = ptr++;
}

void add_path(int ind, int u) {
  static int ptr = 0;
  path_list[ptr] = { ind, nd[u].path_ptr };
  nd[u].path_ptr = ptr++;
}

void read_input_data() {
  FILE* f = fopen("maxflow.in", "r");
  fscanf(f, "%d %d", &n, &num_paths);
  for (int u = 1; u <= n; u++) {
    nd[u].ptr = nd[u].path_ptr = NIL;
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }

  for (int i = 0; i < num_paths; i++) {
    fscanf(f, "%d %d", &p[i].u, &p[i].v);
    add_path(i, p[i].u);
    add_path(i, p[i].v);
  }
  fclose(f);
}

void mark_differences() {
  for (int i = 0; i < num_paths; i++) {
    nd[p[i].u].load++;
    nd[p[i].v].load++;
    nd[p[i].lca].load--;
    if (p[i].lca != 1) {
      int lca_parent = nd[p[i].lca].parent;
      nd[lca_parent].load--;
    }
  }
}

void sum_differences(int u) {
  for (int ptr = nd[u].ptr; ptr != NIL; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (v != nd[u].parent) {
      sum_differences(v);
      nd[u].load += nd[v].load;
    }
  }
}

int get_max_load() {
  int result = 0;
  for (int u = 1; u <= n; u++) {
    if (nd[u].load > result) {
      result = nd[u].load;
    }
  }
  return result;
}

void write_answer(int answer) {
  FILE* f = fopen("maxflow.out", "w");
  fprintf(f, "%d\n", answer);
  fclose(f);
}

int main() {
  read_input_data();
  offline_lca();
  mark_differences();
  sum_differences(1);
  int answer = get_max_load();
  write_answer(answer);

  return 0;
}
