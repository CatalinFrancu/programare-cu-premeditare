#include <deque>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;

typedef std::deque<int> deque;

std::vector<int> adj[MAX_NODES + 1];
int n, k;
long long answer;

void read_input_data() {
  scanf("%d %d", &n, &k);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    adj[u].push_back(v);
    adj[v].push_back(u);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

// Presupune că src.size() <= dest.size().
void merge_into(deque& src, deque&dest) {
  int min_i = max(0, k - 1 - dest.size());
  int max_i = min(src.size() - 1, k - 2);
  for (int i = min_i; i <= max_i; i++) {
    int j = k - 2 - i;
    answer += (long long)src[i] * dest[j];
  }
  for (int i = 0; i < (int)src.size(); i++) {
    dest[i] += src[i];
  }
}

deque dfs(int u, int parent) {
  deque result = { };
  for (int v: adj[u]) {
    if (v != parent) {
      deque d = dfs(v, u);
      if (d.size() > result.size()) {
        // Întotdeauna folosește swap(), care schimbă pointeri. Niciodată nu
        // folosi atribuirea, care copiază date.
        result.swap(d);
      }
      merge_into(d, result);
    }
  }

  result.push_front(1); // nodul însuși la distanță 0
  if ((int)result.size() >= k + 1) {
    answer += result[k];
  }
  if ((int)result.size() > k + 1) {
    // Nu are rost să ținem statistici peste distanța k.
    result.pop_back();
  }
  return result;
}

void write_answer() {
  printf("%lld\n", answer);
}

int main() {
  read_input_data();
  dfs(1, 0);
  write_answer();

  return 0;
}
