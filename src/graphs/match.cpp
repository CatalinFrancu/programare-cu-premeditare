#include <stdio.h>

const int MAX_NODES = 252; // m + n + 2
const int MAX_EDGES = 1'400; // 2 × (m + 2m + 2n)
const int NIL = -1;

struct edge {
  short cap, cost;
  short v, next;
};

struct node {
  short adj;
  bool vis;
};

node nd[MAX_NODES + 1];
edge e[2 * MAX_EDGES];
int n, m, source, sink, max_flow, min_cost, epos;

void add_edge(short u, short v, short cap, short cost) {
  e[epos] = { cap, cost, v, nd[u].adj };
  nd[u].adj = epos++;
}

void add_edge_pair(short u, short v, short cap, short cost) {
  add_edge(u, v, cap, cost);
  add_edge(v, u, 0, -cost);
}

void read_data_and_make_graph() {
  epos = 0;
  scanf("%d %d", &n, &m);
  source = m + n;
  sink = m + n + 1;
  for (int u = 0; u < m + n + 2; u++) {
    nd[u].adj = NIL;
  }

  // Creează întîi muchiile de la jucători la destinație, ca să le aflăm ușor
  // ID-urile cînd le vom manipula capacitățile.
  for (int u = 0; u < n; u++) {
    add_edge_pair(m + u, sink, 0, 0); // vor fi folosite doar în FMCM
    add_edge_pair(m + u, sink, 0, 1); // capacitatea va crește în faza I
  }
  for (int i = 0; i < m; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge_pair(source, i, 1, 0);
    add_edge_pair(i, m + u - 1, 1, 0);
    add_edge_pair(i, m + v - 1, 1, 0);
  }
}

// Crește capacitățile pe muchiile jucător-destinație de cost 1.
void increase_sink_capacities() {
  for (int u = 0; u < n; u++) {
    e[4 * u + 2].cap++;
  }
}

void reset_dfs() {
  for (int u = 0; u < m + n + 2; u++) {
    nd[u].vis = false;
  }
}

// Caută un drum spre destinație. Dacă îl găsește, ajustează capacitățile.
bool dfs(int u, bool cost_zero_only) {
  nd[u].vis = true;
  if (u == sink) {
    return true;
  }

  bool found = false;
  int pos = nd[u].adj;
  while (!found && (pos != NIL)) {
    int v = e[pos].v;
    if (e[pos].cap && !nd[v].vis && (!cost_zero_only || !e[pos].cost)) {
      found = dfs(v, cost_zero_only);
      e[pos].cap -= found;
      e[pos ^ 1].cap += found;
    }
    pos = e[pos].next;
  }

  return found;
}

void ford_fulkerson() {
  bool augment;
  do {
    reset_dfs();
    augment = dfs(source, false);
    max_flow += augment;
  } while (augment);
}

// Capacitățile pe muchiile de la jucători la destinație semnifică punctajul
// maxim permis. Crește simultan aceste capacități cu cîte o unitate pînă cînd
// poate obține un flux de mărime cel puțin m.
int find_min_max_score() {
  int result = 0;
  max_flow = 0;

  do {
    result++;
    increase_sink_capacities();
    ford_fulkerson();
  } while (max_flow < m);

  return result;
}

// Crește capacitatea muchiilor jucător-destinație de cost 0 și o scade pe cea
// a muchiilor de cost 1.
void increase_cost_0_capacities() {
  for (int u = 0; u < n; u++) {
    // 4u, 4u+1 : muchia de cost 0 + reziduala
    // 4u+2, 4u+3: muchia de cost 1 + reziduala
    if (e[4 * u + 3].cap) {
      // Există capacitate reziduală pe muchia de cost 1, deci putem muta
      // această unitate de flux pe muchia de cost 0.
      e[4 * u + 3].cap--;
      e[4 * u + 1].cap++;
      min_cost--;
    } else {
      // Deja pompăm tot fluxul pe muchia de cost 0, deci doar reglăm
      // capacitățile.
      e[4 * u + 2].cap--;
      e[4 * u].cap++;
    }
  }
}

// Caută un ciclu de cost negativ care conține destinația. Dacă îl găsește,
// actualizează fluxul.
bool find_cycle(int u) {
  reset_dfs();
  return dfs(u, true);
}

void find_negative_cost_cycles() {
  int pos = nd[sink].adj;
  while (pos != NIL) {
    if ((e[pos].cost == -1) && e[pos].cap && find_cycle(e[pos].v)) {
      e[pos].cap--;
      e[pos ^ 1].cap++;
      min_cost--;
    } else {
      pos = e[pos].next;
    }
  }
}

// Crește simultan capacitățile pe muchiile jucător-sursă de cost 0 cît timp
// le poate satura.
int find_max_min_score(int max) {
  int x = 0;
  min_cost = m; // faza I a pompat flux doar pe muchii de cost 1
  do {
    x++;
    increase_cost_0_capacities();
    find_negative_cost_cycles();
  } while (min_cost == m - n * x);
  return x - 1;
}

void solve_test() {
  read_data_and_make_graph();
  int min_max_score = find_min_max_score();
  int max_min_score = find_max_min_score(min_max_score);
  printf("%d %d\n", min_max_score, max_min_score);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    solve_test();
  }

  return 0;
}
