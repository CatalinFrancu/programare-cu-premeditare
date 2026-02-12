#include <stdio.h>
#include <unordered_set>

const int MAX_NODES = 250'000;
const int MAX_EDGES = 250'000;
const int MAX_TRICKS = 250'000;
const int INFINITY = 2 * MAX_TRICKS;

struct edge_set {
  std::unordered_set<long long> set;

  void insert(int u, int v) {
    set.insert((long long)u * (MAX_NODES + 1) + v);
  }

  bool contains(int u, int v) {
    return set.contains((long long)u * (MAX_NODES + 1) + v);
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;       // lista de adiacență
  int first_pos; // prima apariție a acestui nod în t[]
  int comp;      // numărul CTC a acestui nod

  // cîmpuri pentru algoritmul lui Tarjan pentru CTC
  int time_in;
  int low;
  bool on_stack;
};

// Un interval de indici din t[] pe care o CTC îi poate servi.
struct range {
  int l, r;

  void minimize(range other) {
    if (other.l < l) {
      *this = other;
    }
  }

  // this = intervalul subarborelui nodului curent
  // child = cel mai timpuriu interval al oricărei CTC descendente
  void combine(range child) {
    if (child.l < l) {
      *this = child;
    } else if (child.l == r + 1) {
      r = child.r;
    }
  }
};

node nd[MAX_NODES + 1];
cell list[MAX_EDGES + 1];
range scc[MAX_NODES + 1];
int st[MAX_NODES], ss; // stiva DFS
int t[MAX_TRICKS + 1]; // santinelă la t[num_tricks]
edge_set eset;
int num_comps;

void minimize(int& x, int y) {
  if (y < x) {
    x = y;
  }
}

void read_data() {
  int num_tricks, n, num_edges, u, v;

  scanf("%d %d", &num_tricks, &n);
  for (int u = 1; u <= n; u++) {
    nd[u].first_pos = INFINITY;
  }
  for (int i = 1; i <= num_tricks; i++) {
    scanf("%d", &t[i]);
    minimize(nd[t[i]].first_pos, i);
  }

  scanf("%d", &num_edges);
  for (int i = 1; i <= num_edges; i++) {
    scanf("%d %d", &u, &v);
    eset.insert(u, v);
    list[i] = { v, nd[u].adj };
    nd[u].adj = i;
  }
}

void process_scc(int last) {
  int v;
  num_comps++;
  range& c = scc[num_comps]; // syntactic sugar
  c.l = INFINITY;

  // Stiva conține nodurile din CTC. Ia prima apariție în t[].
  do {
    v = st[--ss];
    nd[v].comp = num_comps;
    nd[v].on_stack = false;
    minimize(c.l, nd[v].first_pos);
  } while (v != last);

  // Extinde intervalul la dreapta.
  c.r = c.l;
  while (nd[t[c.r + 1]].comp == num_comps) {
    c.r++;
  }
}

range dfs(int u) {
  static int time = 0;
  nd[u].time_in = nd[u].low = ++time;
  nd[u].on_stack = true;
  st[ss++] = u;

  range result = { INFINITY, INFINITY };

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (!nd[v].time_in) {
      // Explorează un nod nou.
      result.minimize(dfs(v));
      minimize(nd[u].low, nd[v].low);
    } else if (nd[v].on_stack) {
      // Muchie înapoi.
      minimize(nd[u].low, nd[v].time_in);
    } else {
      // Muchie la o CTC explorată anterior. Acesta este singurul moment cînd
      // schimbăm rezultatul pentru CTC curentă. Operațiile return propagă
      // aceste valori pînă la rădăcina CTC.
      result.minimize(scc[nd[v].comp]);
    }
  }

  if (nd[u].low == nd[u].time_in) {
    // u este rădăcina unei CTC.
    process_scc(u);
    scc[num_comps].combine(result);
    return scc[num_comps];
  } else {
    return result;
  }
}

void compute_score() {
  // Nodul de start este t[1] și aparține lui scc[num_comps].
  int score = 2;
  for (int i = 2; i <= scc[num_comps].r; i++) {
    score += 1 + eset.contains(t[i - 1], t[i]);
  }
  printf("%d\n", score);
}

int main() {
  read_data();
  dfs(t[1]);
  compute_score();

  return 0;
}
