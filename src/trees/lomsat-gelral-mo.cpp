#include <algorithm>
#include <math.h>
#include <stdio.h>

const int MAX_NODES = 100'000;

struct list {
  int val, next;
};

struct node {
  int color, orig_pos;
  int start, finish;
  int ptr; // lista de adiacență
  long long sum;
};

struct accountant {
  int* color;
  int f[MAX_NODES + 1];       // frecvența fiecărei culori
  int c[MAX_NODES + 1];       // numărul de culori avînd fiecare frecvență
  long long s[MAX_NODES + 1]; // suma culorilor avînd fiecare frecvență
  int left, right, max_f;

  void init(int* color) {
    this->color = color;
    left = 1; right = 0; // gol
  }

  void change_frequency(int x, int delta) {
    c[f[x]]--;
    s[f[x]] -= x;
    f[x] += delta;
    c[f[x]]++;
    s[f[x]] += x;
  }

  void include(int x) {
    change_frequency(x, +1);
    if (f[x] > max_f) {
      max_f++;
    }
  }

  void exclude(int x) {
    change_frequency(x, -1);
    if (!c[max_f]) {
      max_f--;
    }
  }

  long long query(int l, int r) {
    while (right < r) {
      include(color[++right]);
    }
    while (right > r) {
      exclude(color[right--]);
    }
    while (left < l) {
      exclude(color[left++]);
    }
    while (left > l) {
      include(color[--left]);
    }
    return s[max_f];
  }
};

list adj[2 * MAX_NODES];
node n[MAX_NODES + 1];
int color[MAX_NODES + 1];
accountant acc;
int num_nodes, block_size;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  adj[ptr] = { v, n[u].ptr };
  n[u].ptr = ptr++;
}

void read_input_data() {
  scanf("%d", &num_nodes);

  for (int u = 1; u <= num_nodes; u++) {
    scanf("%d", &n[u].color);
    n[u].orig_pos = u;
  }

  for (int i = 0; i < num_nodes - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void dfs(int u) {
  static int time = 0;

  n[u].start = ++time;
  color[time] = n[u].color;

  for (int ptr = n[u].ptr; ptr; ptr = adj[ptr].next) {
    int v = adj[ptr].val;
    if (!n[v].start) {
      dfs(v);
    }
  }

  n[u].finish = time;
}

void sort_in_mo_order() {
  std::sort(n + 1, n + num_nodes + 1, [](node& u, node& v) {
    int bu = u.start / block_size;
    int bv = v.start / block_size;
    if (bu != bv) {
      return bu < bv;
    } else if (bu % 2) {
      return u.finish < v.finish;
    } else {
      return u.finish > v.finish;
    }
  });
}

void sort_in_original_order() {
  std::sort(n + 1, n + num_nodes + 1, [](node& u, node& v) {
    return u.orig_pos < v.orig_pos;
  });
}

void process_queries() {
  block_size = sqrt(num_nodes);
  sort_in_mo_order();
  acc.init(color);
  for (int u = 1; u <= num_nodes; u++) {
    n[u].sum = acc.query(n[u].start, n[u].finish);
  }
  sort_in_original_order();
}

void write_output_data() {
  for (int u = 1; u <= num_nodes; u++) {
    printf("%lld ", n[u].sum);
  }
  printf("\n");
}

int main() {
  read_input_data();
  dfs(1);

  // acum fiecare nod este și o interogare [start, finish]
  process_queries();
  write_output_data();

  return 0;
}
