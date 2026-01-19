#include "flareon.h"
#include <vector>

const int MAX_NODES = 200'000;

struct node {
  std::vector<int> adj;
  std::vector<int> pow; // puterile flăcărilor cu originea aici
  int size;
};

static node nd[MAX_NODES + 1];
static long long ans[MAX_NODES + 1];

struct plume_tracker {
  // Date din punctul de vedere al centroidului, pe măsură ce flăcările urcă
  // în subarbore și ajung la centroid.
  //
  // Frecvențele flăcărilor indexate după puterea rămasă. Nu ținem evidența
  // puterilor mai mari decît n, deoarece acelea nu se vor stinge niciodată.
  int freq[MAX_NODES];
  // Numărul de flăcări.
  int cnt;
  // Puterea totală.
  long long sum;

  // Mărimea subarborelui curent. Ne trebuie ca să ne încadrăm în efort
  // O(mărimea_subarborelui), nu O(n).
  int sts;

  void init(int sts) {
    this->sts = sts;
    sum = cnt = 0;
    for (int i = 0; i < sts; i++) {
      freq[i] = 0;
    }
  }

  void add_plume(int power, int depth, int sign) {
    if (power > depth) {
      if (power - depth < sts)  {
        freq[power - depth] += sign;
      }
      cnt += sign;
      sum += sign * (power - depth);
    }
    // Altfel nu face nimic: flacăra nu va ajunge la centroid.
  }

  // Colectează flăcările din nodul u și notează contribuția lor la centroid.
  // Cînd sign = +1 includem contribuțiile, iar cînd sign = -1, le excludem.
  void collect(int u, int parent, int depth, int sign) {
    for (int p: nd[u].pow) {
      add_plume(p, depth, sign);
    }

    for (int v: nd[u].adj) {
      if (v != parent) {
        collect(v, u, depth + 1, sign);
      }
    }
  }

  void distribute(int u, int parent, int depth, int cnt, long long sum) {
    // Stinge unele flăcări și redu puterile celorlalte cu cîte 1.
    sum -= cnt;
    cnt -= freq[depth];
    ans[u] += sum;

    for (int v: nd[u].adj) {
      if (v != parent) {
        distribute(v, u, depth + 1, cnt, sum);
      }
    }
  }

  void process_plumes_through(int u) {
    init(nd[u].size);
    collect(u, 0, 0, +1);
    ans[u] +=  sum;

    for (int v: nd[u].adj) {
      // Cînd numărăm efectele flăcărilor într-un subarbore, mai întîi
      // excludem flăcările care provin tocmai din acel subarbore.
      collect(v, u, 1, -1);
      distribute(v, u, 1, cnt, sum);
      collect(v, u, 1, +1);
    }
  }
};

plume_tracker pt;

/**
 * Restul codului (decompose(), size_dfs() și find_centroid()) este șablon. Îl
 * folosim deoarece ne permite să rulăm un DFS din fiecare centroid și să
 * obținem timp O(n log n).
 **/

void size_dfs(int u, int parent) {
  nd[u].size = 1;

  for (int v: nd[u].adj) {
    if (v != parent) {
      size_dfs(v, u);
      nd[u].size += nd[v].size;
    }
  }
}

int find_centroid(int u, int limit) {
  for (int v: nd[u].adj) {
    if ((nd[v].size < nd[u].size) && (nd[v].size > limit)) {
      return find_centroid(v, limit);
    }
  }

  return u;
}

void delete_edge(int u, int v) {
  int i = 0;
  while (nd[u].adj[i] != v) {
    i++;
  }
  nd[u].adj[i] = nd[u].adj.back();
  nd[u].adj.pop_back();
}

void delete_node(int u) {
  for (int v: nd[u].adj) {
    delete_edge(v, u);
  }
}

void decompose(int u) {
  size_dfs(u, 0);
  u = find_centroid(u, nd[u].size / 2);

  // Aceasta este problema pe care încercăm să o rezolvăm.
  pt.process_plumes_through(u);

  delete_node(u);
  for (int v: nd[u].adj) {
    decompose(v);
  }
}

void solve(int n, int m, int* v, int* pos, int* power) {
  for (int u = 2; u <= n; u++) {
    nd[u].adj.push_back(v[u - 2]);
    nd[v[u - 2]].adj.push_back(u);
  }

  for (int i = 0; i < m; i++) {
    nd[pos[i]].pow.push_back(power[i]);
  }

  decompose(1);
  answer(ans + 1); // shiftează vectorul cu 1
}
