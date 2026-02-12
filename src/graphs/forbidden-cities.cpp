#include <stdio.h>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_EDGES = 200'000;
const int MAX_QUERIES = 100'000;

struct node {
  std::vector<int> adj;           // lista de adiacență
  std::vector<int> queries;       // interogări în care eu sînt capătul
  std::vector<int> notifications; // interogări în care eu sînt centrul
  int tin, tout;
  int low;
  bool notify_me;
};

struct query {
  int a, b, c;
  bool red_flag;
};

node nd[MAX_NODES + 1];
query q[MAX_QUERIES + 1];
int n, num_queries;

void read_data() {
  int num_edges, u, v;
  scanf("%d %d %d", &n, &num_edges, &num_queries);

  while (num_edges--) {
    scanf("%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int i = 1; i <= num_queries; i++) {
    scanf("%d %d %d", &q[i].a, &q[i].b, &q[i].c);
    nd[q[i].a].queries.push_back(i);
    nd[q[i].b].queries.push_back(i);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

bool is_ancestor(int u, int v) {
  return (nd[v].tin >= nd[u].tin) && (nd[v].tout <= nd[u].tout);
}

void send_notifications(int u) {
  for (int ind: nd[u].queries) {
    int c = q[ind].c;
    if (nd[c].notify_me) {
      nd[c].notifications.push_back(ind);
    }
  }
}

void receive_notifications(int u, int child) {
  for (int ind: nd[u].notifications) {
    if (is_ancestor(child, q[ind].a) ^ is_ancestor(child, q[ind].b)) {
      q[ind].red_flag = true;
    }
  }
}

void clear_notifications(int u) {
  nd[u].notifications.clear();
}

void dfs(int u, int parent) {
  static int time = 0;
  nd[u].tin = nd[u].low = ++time;
  nd[u].notify_me = true;

  for (int v: nd[u].adj) {
    if (!nd[v].tin) {
      dfs(v, u);
      nd[u].low = min(nd[u].low, nd[v].low);

      if (nd[v].low >= nd[u].tin) {
        // v nu îl poate ocoli pe u: u este punct de articulație
        receive_notifications(u, v);
      }
      clear_notifications(u);

    } else if (v != parent) {
      nd[u].low = min(nd[u].low, nd[v].tin);
    }
  }

  send_notifications(u);
  nd[u].notify_me = false;
  nd[u].tout = ++time;
}

void write_answers() {
  for (int i = 1; i <= num_queries; i++) {
    q[i].red_flag |= (q[i].a == q[i].c) || (q[i].b == q[i].c);
    printf(q[i].red_flag ? "NO\n" : "YES\n");
  }
}

int main() {
  read_data();
  dfs(1, 0);
  write_answers();

  return 0;
}
