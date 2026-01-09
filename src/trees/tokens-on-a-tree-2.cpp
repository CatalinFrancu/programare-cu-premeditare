#include <list>
#include <stdio.h>

const int MAX_NODES = 1'000'000;

long long total_moves;

void swap(int& x, int& y) {
  int tmp = x;
  x = y;
  y = tmp;
}

struct node {
  int parent, num_children;
  std::list<int> f; // numărul de monede la adîncimile 0, 1, ...
  bool has_coin;

  void reset(bool has_coin) {
    this->has_coin = has_coin;
    f.clear();
  }

  // Procesează un nod după ce toți fiii săi i-au transmis datele lor.
  void process(node& par) {
    prepend_self();
    trim();
    spill_into(par);
  }

  void prepend_self() {
    if (has_coin) {
      f.push_front(1);
    } else if (!f.empty() && f.back()) {
      f.push_front(1);
      f.back()--;
      total_moves += f.size() - 1;
    }
    // altfel nu există monete în subarbore și nu adăugăm un element 0
  }

  void trim() {
    while (!f.empty() && !f.back()) {
      f.pop_back();
    }
  }

  void spill_into(node& other) {
    if (f.size() > other.f.size()) {
      f.swap(other.f);
    }

    for (auto it = f.begin(), other_it = other.f.begin();
         it != f.end();
         it++, other_it++) {
      *other_it += *it;
    }

    f.clear();
  }
};

node nd[MAX_NODES + 1];
int n;

void read_input_data() {
  scanf("%d ", &n);

  for (int u = 1; u <= n; u++) {
    int c = getchar();
    nd[u].reset(c == '1');
  }

  for (int u = 2; u <= n; u++) {
    scanf("%d", &nd[u].parent);
    nd[nd[u].parent].num_children++;
  }
}

void traverse() {
  for (int u = 1; u <= n; u++) {
    int p = u;
    while (p && !nd[p].num_children) {
      nd[p].process(nd[nd[p].parent]);
      p = nd[p].parent;
      nd[p].num_children--;
    }
  }
}

void run_test() {
  read_input_data();
  total_moves = 0;
  traverse();
  printf("%lld\n", total_moves);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    run_test();
  }

  return 0;
}
