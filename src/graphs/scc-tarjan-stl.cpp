#include <algorithm>
#include <stack>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;

struct node {
  std::vector<int> adj, adjt;
  int time_in;
  int low;
  bool on_stack;
};

node n[MAX_NODES + 1];
std::stack<int> st;  // stiva DFS
std::vector<std::vector<int>> comps;
int num_nodes, time;

void read_graph() {
  FILE* f = fopen("ctc.in", "r");
  int num_edges, u, v;
  fscanf(f, "%d %d", &num_nodes, &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    fscanf(f, "%d %d", &u, &v);
    n[u].adj.push_back(v);
  }
  fclose(f);
}

void pop_scc(int last) {
  std::vector<int> c;
  int v;
  do {
    v = st.top();
    st.pop();
    c.push_back(v);
    n[v].on_stack = false;
  } while (v != last);
  comps.push_back(c);
}

void dfs(int u) {
  n[u].time_in = n[u].low = ++time;
  n[u].on_stack = true;
  st.push(u);

  for (auto v: n[u].adj) {
    if (!n[v].time_in) {
      // Traversează fiul alb și notează cît poate să urce.
      dfs(v);
      n[u].low = std::min(n[u].low, n[v].low);
    } else if (n[v].on_stack) {
      // Putem urca pînă la nivelul lui v
      n[u].low = std::min(n[u].low, n[v].time_in);
    }
  }

  // Dacă niciun nod din subarborele lui u nu poate urca mai sus decît u,
  // atunci stiva de la u pînă la vîrf este o CTC.
  if (n[u].low == n[u].time_in) {
    pop_scc(u);
  }
}

void dfs_driver() {
  for (int u = 1; u <= num_nodes; u++) {
    if (!n[u].time_in) {
      dfs(u);
    }
  }
}

void write_components() {
  FILE* f = fopen("ctc.out", "w");
  fprintf(f, "%lu\n", comps.size());
  for (auto vec: comps)  {
    for (auto u: vec) {
      fprintf(f, "%d ", u);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

int main() {
  read_graph();
  dfs_driver();
  write_components();

  return 0;
}
