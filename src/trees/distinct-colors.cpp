#include <stdio.h>
#include <unordered_map>

const int MAX_NODES = 200'000;

struct cell {
  int v, next;
};

struct node {
  int color, distinct;
  int adj; // lista de adiacență
};

// Un arbore Fenwick extins cu culori. Cînd colorăm un bit cu o culoare,
// arborele șterge automat apariția anterioară a culorii.
struct fenwick_tree {
  int v[MAX_NODES + 1];
  std::unordered_map<int, int> last_pos;
  int n, total;

  void init(int n) {
    this->n = n;
    total = 0;
  }

  void add(int pos, int val) {
    total += val;
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void colorize(int pos, int color) {
    auto last = last_pos.find(color);
    if (last != last_pos.end()) {
      add(last->second, -1);
      last->second = pos;
    } else {
      last_pos[color] = pos;
    }
    add(pos, +1);
  }

  // count[pos..n] = total - count[1..pos-1]
  int suffix_count(int pos) {
    int s = total;
    pos--;
    while (pos) {
      s -= v[pos];
      pos &= pos - 1;
    }
    return s;
  }
};

cell list[2 * MAX_NODES];
node nd[MAX_NODES + 1];
fenwick_tree fen;
int n;

void add_neighbor(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_input_data() {
  scanf("%d", &n);

  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].color);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_neighbor(u, v);
    add_neighbor(v, u);
  }
}

void dfs(int u) {
  static int time = 0;
  int entry_time = ++time;

  fen.colorize(time, nd[u].color);
  nd[u].color = 0; // previne revizitarea

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (nd[v].color) {
      dfs(v);
    }
  }

  nd[u].distinct = fen.suffix_count(entry_time);
}

void write_output_data() {
  for (int u = 1; u <= n; u++) {
    printf("%d ", nd[u].distinct);
  }
  printf("\n");
}

int main() {
  read_input_data();
  fen.init(n);
  dfs(1);
  write_output_data();

  return 0;
}
