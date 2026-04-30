#include <algorithm>
#include <set>
#include <stdio.h>

typedef std::multiset<int> multiset;

const int MAX_N = 150'000;
const long long INF = 1'000'000'000'000'000ll;

// Structura grafului. A_ sînt clasele persoanelor, iar B_ sînt rolurile.
const int S = 0;
const int A_C = 1;
const int A_M = 2;
const int A_MC = 3;
const int A_I = 4;
const int A_IC = 5;
const int A_IM = 6;
const int A_IMC = 7;
const int B_C = 8;
const int B_M = 9;
const int B_I = 10;
const int B_IM = 11;
const int T = 12;
const int NIL = -1;
const int NUM_CLASSES = 8;
const int NUM_NODES = 13;
const int FIRST_A = 1;
const int LAST_A = 7;
const int FIRST_B = 8;
const int LAST_B = 11;

const int ADJ[NUM_NODES][NUM_NODES] = {
  /* S */     { A_C, A_M, A_MC, A_I, A_IC, A_IM, A_IMC, NIL },
  /* A_C */   { S, B_C, NIL },
  /* A_M */   { S, B_M, B_IM, NIL },
  /* A_MC */  { S, B_C, B_M, B_IM, NIL },
  /* A_I */   { S, B_I, B_IM, NIL },
  /* A_IC */  { S, B_C, B_I, B_IM, NIL },
  /* A_IM */  { S, B_M, B_I, B_IM, NIL },
  /* A_IMC */ { S, B_C, B_M, B_I, B_IM, NIL },
  /* B_C */   { A_C, A_MC, A_IC, A_IMC, T, NIL },
  /* B_M */   { A_M, A_MC, A_IM, A_IMC, T, NIL },
  /* B_I */   { A_I, A_IC, A_IM, A_IMC, T, NIL },
  /* B_IM */  { A_M, A_MC, A_I, A_IC, A_IM, A_IMC, T, NIL },
  /* T */     { B_C, B_M, B_I, B_IM, NIL },
};

struct person {
  int attr, risk;
};

// O rețea de flux. Ocupă circa 700 de octeți, deci o putem copia cu totul la
// nevoie.
struct network_flow {
  int cap[NUM_NODES][NUM_NODES];
  bool seen[NUM_NODES];
  int flow;

  void build_from_persons(person* p, int n) {
    for (int i = 0; i < n; i++) {
      cap[S][p[i].attr]++;
    }

    // capacități infinite de la A la B
    for (int u = FIRST_A; u <= LAST_A; u++) {
      for (int i = 0; ADJ[u][i] != NIL; i++) {
        int v = ADJ[u][i];
        if ((v >= FIRST_B) && (v <= LAST_B)) {
          cap[u][v] = n;
        }
      }
    }
  }

  void increase_sink_capacities() {
    for (int u = FIRST_B; u <= LAST_B; u++) {
      cap[u][T]++;
    }
  }

  void increase_source_capacity(int a) {
    cap[S][a]++;
  }

  void reset_dfs() {
    for (int u = 0; u < NUM_NODES; u++) {
      seen[u] = false;
    }
  }

  bool dfs(int u) {
    seen[u] = true;
    if (u == T) {
      return true;
    }

    for (int i = 0; ADJ[u][i] != NIL; i++) {
      int v = ADJ[u][i];
      if (cap[u][v] && !seen[v] && dfs(v)) {
        cap[u][v]--;
        cap[v][u]++;
        return true;
      }
    }

    return false;
  }

  void ford_fulkerson() {
    bool reachable;
    do {
      reset_dfs();
      reachable = dfs(S);
      flow += reachable;
    } while (reachable);
  }

  void remove_unit(int a_node) {
    if (cap[S][a_node]) {
      // Există persoane cu aceste atribute încă necuplate. Elimină pur și
      // simplu una dintre aceste persoane.
      cap[S][a_node]--;
    } else {
      // Toate persoanele cu aceste atribute sînt cuplate. Caută un b către
      // care se duce fluxul și elimină unitatea S->a->b->T.
      int b = FIRST_B;
      while (!cap[b][a_node]) {
        b++;
      }

      cap[a_node][S]--;
      cap[a_node][b]++;
      cap[b][a_node]--;
      cap[b][T]++;
      cap[T][b]--;
      flow--;
    }
  }
};

struct team_collection {
  multiset risks_in[NUM_CLASSES];
  multiset risks_out[NUM_CLASSES];
  long long total_in_risk; // menținut incremental

  void include(person p) {
    risks_in[p.attr].insert(p.risk);
    total_in_risk += p.risk;
  }

  void exclude(person p) {
    risks_out[p.attr].insert(p.risk);
  }

  // Înlocuiește un risc (care garantat există).
  void replace_value(int attr, int old_risk, int new_risk) {
    multiset& rin = risks_in[attr];
    multiset& rout = risks_out[attr];

    auto it = rin.find(old_risk);
    if (it != rin.end()) {
      rin.erase(it);
      rin.insert(new_risk);
      total_in_risk += new_risk - old_risk;
    } else {
      it = rout.find(old_risk);
      rout.erase(it);
      rout.insert(new_risk);
    }
  }

  // Asigură-te că nicio valoare din risks_in nu este mai mare decît nicio
  // valoare din risks_out.
  void resort_sets(int attr) {
    multiset& rin = risks_in[attr];
    multiset& rout = risks_out[attr];

    if (!rin.empty() && !rout.empty()) {
      int last_in = *rin.rbegin();
      int first_out = *rout.begin();
      if (last_in > first_out) {
        rin.erase(std::prev(rin.end()));
        rout.erase(rout.begin());
        rin.insert(first_out);
        rout.insert(last_in);
        total_in_risk += first_out - last_in;
      }
    }
  }

  // Mută un element de la excluse la incluse.
  void move_in(int attr) {
    auto it = risks_out[attr].begin();
    total_in_risk += *it;
    risks_in[attr].insert(*it);
    risks_out[attr].erase(it);
  }

  // Mută un element de la incluse la excluse.
  void move_out(int attr) {
    auto it = std::prev(risks_in[attr].end());
    total_in_risk -= *it;
    risks_out[attr].insert(*it);
    risks_in[attr].erase(it);
  }
};

network_flow g;
team_collection tc;
person p[MAX_N];
int ind[MAX_N]; // pentru sortarea după risc
int n, num_teams;

void read_people() {
  scanf("%d ", &n);
  for (int i = 0; i < n; i++) {
    p[i].attr = getchar() - '0';
    p[i].attr += 2 * (getchar() - '0');
    p[i].attr += 4 * (getchar() - '0');
    scanf("%d ", &p[i].risk);
  }
}

void find_num_teams() {
  network_flow orig_g;
  g.build_from_persons(p, n);
  do {
    orig_g = g;
    num_teams++;
    g.increase_sink_capacities();
    g.ford_fulkerson();
  } while (g.flow == 4 * num_teams);

  // Ultima creștere a modificat graful, apoi a eșuat.
  num_teams--;
  g = orig_g;
}

void sort_by_risk() {
  for (int i = 0; i < n; i++) {
    ind[i] = i;
  }
  std::sort(ind, ind + n, [](int a, int b) {
    return p[a].risk > p[b].risk;
  });
}

void find_min_cost_teams() {
  sort_by_risk();

  for (int i = 0; i < n; i++) {
    person& dude = p[ind[i]];
    network_flow orig_g = g;
    g.remove_unit(dude.attr);
    g.ford_fulkerson();
    if (g.flow == 4 * num_teams) {
      tc.exclude(dude);
    } else {
      tc.include(dude);
      g = orig_g;
    }
  }

  printf("%d %lld\n", num_teams, tc.total_in_risk);
}

// Încearcă să înlocuiască ultimul element inclus din del cu primul element
// exclus din ins. Operează pe graful gc, pe care îl inițializează cu o copie
// a lui g.
//
// Returnează noul risc, sau INF dacă înlocuirea este imposibilă sau neoptimă.
long long try_swap_pair(int del, int ins, network_flow* gc, long long best_so_far) {
  if (del == ins ||
      tc.risks_in[del].empty() ||
      tc.risks_out[ins].empty()) {
    return INF;
  }

  int del_risk = *tc.risks_in[del].rbegin();
  int ins_risk = *tc.risks_out[ins].begin();
  long long new_risk = tc.total_in_risk - del_risk + ins_risk;

  if (new_risk >= best_so_far) {
    return INF; // Este o schimbare în rău.
  }

  // Acum are sens să încercăm să refacem fluxul.
  *gc = g;
  gc->remove_unit(del);
  gc->increase_source_capacity(ins);
  gc->ford_fulkerson();

  return (gc->flow == 4 * num_teams) ? new_risk : INF;
}

// Încearcă pe rînd să schimbe fiecare pereche (del, ins) unde first_del ≤ del
// ≤ last_del și first_ins ≤ ins ≤ last_ins. Unul dintre aceste intervale are
// lungime 0.
void try_swap(int first_del, int last_del, int first_ins, int last_ins) {
  int step_del = (last_del > first_del);
  int step_ins = (last_ins > first_ins);

  network_flow best_g = g, gc = g;
  long long best_risk = tc.total_in_risk;
  int best_del = 0, best_ins = 0;

  for (int del = first_del, ins = first_ins;
       (del <= last_del) && (ins <= last_ins);
       del += step_del, ins += step_ins) {
    long long new_risk = try_swap_pair(del, ins, &gc, best_risk);
    if (new_risk < best_risk) {
      best_risk = new_risk;
      best_g = gc;
      best_del = del;
      best_ins = ins;
    }
  }

  if (best_risk < tc.total_in_risk) {
    g = best_g;
    tc.move_out(best_del);
    tc.move_in(best_ins);
  }
}

void try_swap_out(int attr) {
  try_swap(attr, attr, FIRST_A, LAST_A);
}

void try_swap_in(int attr) {
  try_swap(FIRST_A, LAST_A, attr, attr);
}

void replace(int attr, int old_risk, int new_risk) {
  tc.replace_value(attr, old_risk, new_risk);
  tc.resort_sets(attr);
  if (new_risk > old_risk) {
    try_swap_out(attr);
  } else if (new_risk < old_risk) {
    try_swap_in(attr);
  }
}

void process_queries() {
  int num_queries, pid, risk;
  scanf("%d", &num_queries);
  while (num_queries--) {
    scanf("%d %d", &pid, &risk);
    pid--;
    replace(p[pid].attr, p[pid].risk, risk);
    p[pid].risk = risk;
    printf("%lld\n", tc.total_in_risk);
  }
}

int main() {
  read_people();
  find_num_teams();
  find_min_cost_teams();
  process_queries();

  return 0;
}
