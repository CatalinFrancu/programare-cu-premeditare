// Abordare cu DFS exclusiv.
#include <stdio.h>
#include <vector>

const int MAX_NODES = 1'000'000;

struct node {
  std::vector<int> adj;
  int depth;
  int heavy;
  int answer;
};

node nd[MAX_NODES + 1];
int pop[MAX_NODES], min_ind;
int n;

void read_tree() {
  scanf("%d", &n);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
}

// Calculează pentru fiecare nod adîncimea și fiul heavy. Șterge părintele din
// lista de adiacență a fiecărui nod pentru a accelera DFS-urile viitoare.
// Returnează mărimea subarborelui.
int size_dfs(int u, int parent) {
  int size = 1, max_c_size = 0;

  unsigned i = 0;
  while (i < nd[u].adj.size()) {
    int v = nd[u].adj[i];
    if (v == parent) {
      nd[u].adj[i] = nd[u].adj.back();
      nd[u].adj.pop_back();
    } else {
      nd[v].depth = 1 + nd[u].depth;
      int c = size_dfs(v, u);
      size += c;
      if (!nd[u].heavy || (c > max_c_size)) {
        nd[u].heavy = v;
        max_c_size = c;
      }
      i++;
    }
  }

  return size;
}

void insert_node(int u) {
  int d = nd[u].depth;
  pop[d]++;
  if ((pop[d] > pop[min_ind]) ||
      ((pop[d] == pop[min_ind]) && (d < min_ind))) {
    min_ind = d;
  }
}

void insert_subtree(int u) {
  insert_node(u);
  for (int v: nd[u].adj) {
    insert_subtree(v);
  }
}

void delete_subtree(int u) {
  pop[nd[u].depth]--;
  for (int v: nd[u].adj) {
    delete_subtree(v);
  }
}

void dfs(int u) {
  for (int v: nd[u].adj) {
    if (v != nd[u].heavy) {
      int save_min_ind = min_ind;
      dfs(v);
      delete_subtree(v);
      min_ind = save_min_ind;
    }
  }

  if (nd[u].heavy) {
    dfs(nd[u].heavy);
  }

  for (int v: nd[u].adj) {
    if (v != nd[u].heavy) {
      insert_subtree(v);
    }
  }

  insert_node(u);
  nd[u].answer = min_ind - nd[u].depth;
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    printf("%d\n", nd[u].answer);
  }
}

int main() {
  read_tree();
  size_dfs(1, 0);
  dfs(1);
  write_answers();

  return 0;
}
