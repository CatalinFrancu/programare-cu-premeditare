// Abordare small-to-large.
#include <vector>
#include <stdio.h>

const int MAX_NODES = 1'000'000;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int answer;
};

// Stochează histograma unui subarbore, adică numerele de noduri la fiecare
// adîncime relativă la rădăcina subarborelui. Vectorul h este *răsturnat*:
// h[0] stochează numărul de noduri cele mai depărtate de rădăcină, iar
// h.back() stochează 1 pentru rădăcina însăși.
//
// Histograma stochează și indicele unde h este maximizat. Ca departajare,
// preferăm indicele mai apropiat de rădăcină.
struct histogram {
  std::vector<int> h;
  int max_pos;

  histogram(): h(), max_pos(0) {
  }

  int size() {
    return h.size();
  }

  void swap(histogram& other) {
    h.swap(other.h);
    int tmp = max_pos;
    max_pos = other.max_pos;
    other.max_pos = tmp;
  }

  void include(histogram& src) {
    int shift = h.size() - src.h.size();
    // Aliniază ultimele elemente ale lui this și src.
    for (unsigned i = 0, j = shift; i < src.h.size(); i++, j++) {
      h[j] += src.h[i];
      optimize(j);
    }
  }

  void optimize(int pos) {
    if ((h[pos] > h[max_pos]) ||
        ((h[pos] == h[max_pos]) && (pos > max_pos))) {
      max_pos = pos;
    }
  }

  void prepend_root() {
    h.push_back(1);
    optimize(h.size() - 1);
  }

  int get_real_max_pos() {
    return h.size() - 1 - max_pos;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
int n;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d", &n);

  for (int i = 1; i < n; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

histogram dfs(int u, int parent) {
  histogram result;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      histogram h = dfs(v, u);
      if (h.size() > result.size()) {
        result.swap(h);
      }
      result.include(h);
    }
  }

  result.prepend_root();
  nd[u].answer = result.get_real_max_pos();

  return result;
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    printf("%d\n", nd[u].answer);
  }
}

int main() {
  read_tree();
  dfs(1, 0);
  write_answers();

  return 0;
}
