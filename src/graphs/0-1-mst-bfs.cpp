#include <unordered_set>
#include <queue>
#include <stdio.h>

const int MAX_NODES = 100'000;

std::unordered_set<int> adj[MAX_NODES + 1];
int unseen[MAX_NODES], num_unseen;
int n, answer;

void read_graph() {
  int m, u, v;
  scanf("%d %d", &n, &m);
  while (m--) {
    scanf("%d %d", &u, &v);
    adj[u].insert(v);
    adj[v].insert(u);
  }
}

void init_seen() {
  for (int u = 1; u <= n; u++) {
    unseen[num_unseen++] = u;
  }
}

void bfs(int u) {
  std::queue<int> q;
  q.push(u);
  while (!q.empty()) {
    int u = q.front();
    q.pop();

    int i = 0;
    while (i < num_unseen) {
      int v = unseen[i];
      if (adj[u].find(v) == adj[u].end()) {
        q.push(v);
        unseen[i] = unseen[--num_unseen];
      } else {
        i++;
      }
    }
  }
}

void bfs_wrapper() {
  int num_comps = 0;
  while (num_unseen) {
    num_comps++;
    int u = unseen[--num_unseen];
    bfs(u);
  }
  answer = num_comps - 1;
}

void write_answer() {
  printf("%d\n", answer);
}

int main() {
  read_graph();
  init_seen();
  bfs_wrapper();
  write_answer();

  return 0;
}
