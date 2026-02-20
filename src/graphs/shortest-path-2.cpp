#include <algorithm>
#include <queue>
#include <stdio.h>

const int MAX_NODES = 3'000;
const int MAX_EDGES = 20'000;
const int MAX_TRIPLETS = 100'000;

typedef unsigned short u16;

struct edge {
  u16 u, v;       // capete
  int bad;        // primul indice în t[] pentru <u,v,*>
  u16 d;          // distanța
  u16 prev;       // indicele muchiei precedente pe calea cea mai scurtă
};

struct triplet {
  u16 u, v, w;
};

edge e[2 * MAX_EDGES + 1];
// adj[u] = primul indice în e[] al muchiilor ce pornesc din u
u16 adj[MAX_NODES + 1];
triplet t[MAX_TRIPLETS + 1];
std::queue<int> q;
int n, m, num_triplets, last_edge;

void read_data() {
  scanf("%d %d %d\n", &n, &m, &num_triplets);

  for (int i = 0; i < m; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    e[2 * i + 1].u = u;
    e[2 * i + 1].v = v;
    e[2 * i + 2].u = v;
    e[2 * i + 2].v = u;
  }

  for (int i = 0; i < num_triplets; i++) {
    scanf("%hu %hu %hu", &t[i].u, &t[i].v, &t[i].w);
  }
}

void sort_data() {
  std::sort(e + 1, e + 2 * m + 1, [](edge& a, edge& b) {
    return (a.u < b.u) ||
      ((a.u == b.u) && (a.v < b.v));
  });

  std::sort(t, t + num_triplets, [](triplet& a, triplet& b) {
    return (a.u < b.u) ||
      ((a.u == b.u) && (a.v < b.v)) ||
      ((a.u == b.u) && (a.v == b.v) && (a.w < b.w));
  });
}

// Distribuie tripleții la muchii. Pentru fiecare muchie (u,v) află primul
// triplet <u,v,*>, dacă există.
void distribute_triplets() {
  int j = 0;
  t[num_triplets] = { (u16)(n + 1), (u16)(n + 1), (u16)(n + 1)}; // santinelă

  for (int i = 1; i <= 2 * m; i++) {
    while ((t[j].u < e[i].u) || ((t[j].u == e[i].u) && (t[j].v < e[i].v))) {
      j++;
    }
    e[i].bad = j; // posibil incorect, dar vom verifica mai tîrziu
  }
}

// Construiește listele de adiacență pentru fiecare nod.
void distribute_edges() {
  for (int i = 2 * m; i; i--) {
    adj[e[i].u] = i;
  }
}

void relax_good_successors(int ind) {
  edge& f = e[ind];

  // Interclasează continuările rele pentru f cu muchiile care pornesc din
  // f.v. Relaxează muchiile care nu sînt rele.
  int j = f.bad;
  for (unsigned i = adj[f.v]; e[i].u == f.v; i++) {
    int w = e[i].v;
    while ((t[j].u == f.u) && (t[j].v == f.v) && (t[j].w < w)) {
      j++;
    }

    if (!e[i].d && ((t[j].u != f.u) || (t[j].v != f.v) || (t[j].w != w))) {
      // Nu am găsit niciun triplet <f.u, f.v, w>
      e[i].d = f.d + 1;
      e[i].prev = ind;
      if (e[i].v == n) {
        last_edge = i;
      }
      q.push(i);
    }
  }
}

void bfs() {
  // Inițializează coada cu o muchie fictivă 1-1.
  e[0].u = e[0].v = 1;
  q.push(0);

  while (!q.empty() && !last_edge) {
    unsigned ind = q.front();
    q.pop();
    relax_good_successors(ind);
  }
}

void recover_solution(int ind) {
  if (ind) {
    recover_solution(e[ind].prev);
    printf("%d ", e[ind].u);
  }
}

void write_answer() {
  if (last_edge) {
    printf("%d\n", e[last_edge].d);
    recover_solution(last_edge);
    printf("%d\n", n);
  } else {
    printf("-1\n");
  }
}
int main() {
  read_data();
  sort_data();
  distribute_triplets();
  distribute_edges();
  bfs();
  write_answer();

  return 0;
}
