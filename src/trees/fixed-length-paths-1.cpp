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

// Presupune că src.size() <= dest.size().
void merge_into(deque& src, deque&dest) {
  int min_i = std::max(k - (int)dest.size() + 1, 0);
  int max_i = std::min((int)src.size() - 1, k);
  for (int i = min_i; i <= max_i; i++) {
    answer += (long long)src[i] * dest[k - i];
  }
  for (int i = 0; i < (int)src.size(); i++) {
    dest[i] += src[i];
  }
}

deque dfs(int u, int parent) {
  deque result = { 1 }; // nodul însuși
  for (int v: adj[u]) {
    if (v != parent) {
      deque d = dfs(v, u);
      d.push_front(0);
      if (d.size() > result.size()) {
        // Întotdeauna folosește swap(), care schimbă pointeri. Niciodată nu
        // folosi atribuirea, care copiază date.
        result.swap(d);
      }
      merge_into(d, result);
    }
  }

  if ((int)result.size() > k) {
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
