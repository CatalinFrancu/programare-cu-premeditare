// 1. Mapează numele la numere ca de obicei.
// 2. Rulează un DFS pentru a calcula adîncimile și timpii de intrare/ieșire.
// 3. Rescrie fiecare interogare ca <u,d> ca „interoghează nodurile de nivel
//    depth[u] + d descoperite de DFS între timpii [t_i[u], t_o[u]]”.
// 4. Ordonează interogările după adîncime, apoi după t_o.
// 5. Calculează ordinea BFS. Fiecare interogare corespunde unui interval
//    contiguu în această ordonare.
// 6. Răspunde la intrerogări folosind un AIB ca în problema D-query.
#include <algorithm>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;
const int MAX_NAME_LENGTH = 20;

struct node {
  std::vector<int> children;
  int name;
  int tin, tout;
  int depth;
};

struct name_map {
  std::unordered_map<std::string, int> map;

  int insert_and_get_number(char* s) {
    std::string str(s);
    auto it = map.find(str);
    if (it == map.end()) {
      int result = map.size();
      map[str] = result;
      return result;
    } else {
      return it->second;
    }
  }
};

struct query {
  int orig_index;
  int depth;
  int tin, tout;
};

struct fenwick_tree {
  int v[MAX_NODES + 1];
  int last[MAX_NODES + 1];
  int n;

  void init(int n) {
    this->n = n;
  }

  void add(int pos, int delta) {
    do {
      v[pos] += delta;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void set(int pos, int name) {
    if (last[name]) {
      add(last[name], -1);
    }
    add(pos, +1);
    last[name] = pos;
  }

  int prefix_sum(int pos) {
    int sum = 0;
    while (pos) {
      sum += v[pos];
      pos &= pos - 1;
    }
    return sum;
  }

  int range_sum(int l, int r) {
    return prefix_sum(r) - prefix_sum(l - 1);
  }
};

node nd[MAX_NODES + 1];
int bfs[MAX_NODES + 1];
query q[MAX_QUERIES];
int sol[MAX_QUERIES];
fenwick_tree fen;
int n, num_queries;

void read_tree() {
  name_map map;
  char name[MAX_NAME_LENGTH + 1];
  int parent;

  scanf("%d", &n);
  for (int u = 1; u <= n; u++) {
    scanf("%s %d", name, &parent);
    nd[u].name = map.insert_and_get_number(name);
    nd[parent].children.push_back(u);
  }
}

void dfs(int u) {
  static int time = 0;
  nd[u].tin = time++;

  for (int v: nd[u].children) {
    nd[v].depth = 1 + nd[u].depth;
    dfs(v);
  }

  nd[u].tout = time - 1;
}

void read_queries() {
  scanf("%d", &num_queries);

  for (int i = 0; i < num_queries; i++) {
    int u, depth;
    scanf("%d %d", &u, &depth);
    q[i] = { i, nd[u].depth + depth, nd[u].tin, nd[u].tout };
  }
}

void sort_queries() {
  std::sort(q, q + num_queries, [](query& a, query& b) {
    return (a.depth < b.depth) ||
      ((a.depth == b.depth) && (a.tout < b.tout));
  });
}

void breadth_first_search() {
  int head = 0, tail = 0;
  bfs[tail++] = 0; // pune rădăcina în coadă

  while (head != tail) {
    int u = bfs[head++];
    for (int v: nd[u].children) {
      bfs[tail++] = v;
    }
  }
}

bool query_ends_at(query& q, int k) {
  if (k == n) {
    return true;
  }

  node& u = nd[bfs[k + 1]]; // următorul în BFS
  return (u.depth > q.depth) || ((u.depth == q.depth) && (u.tout > q.tout));
}

// Unde începe această interogare? Caută cel mai din stînga nod din BFS la
// adîncimea q.depth și cu timpul DFS cel puțin q.tin.
int bin_search(query& q, int end) {
  int l = 0, r = end; // (r, l]
  while (r - l > 1) {
    int mid = (r + l) / 2;
    int v = bfs[mid];
    if ((nd[v].depth < q.depth) ||
        (nd[v].tin < q.tin)) {
      l = mid;
    } else {
      r = mid;
    }
  }

  int v = bfs[r];
  return (nd[v].depth == q.depth) && (nd[v].tin >= q.tin)
    ? r
    : (end + 1);
}

void answer_queries_ending_at(int k) {
  static int i = 0;
  while ((i < num_queries) && query_ends_at(q[i], k)) {
    int start = bin_search(q[i], k);
    sol[q[i].orig_index] = fen.range_sum(start, k);
    i++;
  }
}

void answer_queries() {
  fen.init(n);

  for (int i = 1; i <= n; i++) {
    int v = bfs[i];
    fen.set(i, nd[v].name);
    answer_queries_ending_at(i);
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf("%d\n", sol[i]);
  }
}

int main() {
  read_tree();
  dfs(0);
  breadth_first_search();
  read_queries();
  sort_queries();
  breadth_first_search();
  answer_queries();
  write_answers();

  return 0;
}
