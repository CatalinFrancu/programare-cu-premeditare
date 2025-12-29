#include <stdio.h>

const int MAX_NODES = 5'000;
const int MOD = 666'013;

const int ANY = -1;
const int LIAR = 0;
const int TRUTH_TELLER = 1;

// Combinații posibile pentru nodul curent × părintele
const int LIAR_P_LIAR = 0;
const int LIAR_P_TT = 1;
const int TT_P_LIAR = 2;
const int TT_P_TT = 3;

struct cell {
  int v, next;
};

struct node {
  int adj;
  int num_children;
  int resp;       // numărul de vecini mincinoși declarați de u
  int assignment; // valoarea asignată lui u în soluția construită
  int ways[4];    // În cîte moduri putem fixa calitatea nodului și a părintelui?
};

node nd[MAX_NODES + 1];
cell list[2 * MAX_NODES];
int n;

// Vectorul pentru combinatorică. Ne ajunge unul singur, căci îl completăm în
// fiecare nod după ce procesăm fiii. a[i] = numărul de moduri de a avea i fii
// mincinoși. Calitatea părintelui este fixată în momentul apelului. Din
// fiecare fiu vom folosi cîmpurile ways[LIAR_P_LIAR] și ways[TT_P_LIAR]
// pentru a fixa părintele ca mincinos, respectiv ways[LIAR_P_TT] și
// ways[TT_P_TT] pentru a fixa părintele ca sincer.
//
// Îl îmbrăcăm într-un struct ca să oferim funcții getter pentru indici
// incorecți.
struct combo {
  long long a[MAX_NODES + 1];
  int curr_node;

  void merge_children(int u, int parent, int c_liar, int c_tt) {
    curr_node = u;

    // Înainte de a procesa fiii, avem un singur mod de a avea 0 mincinoși.
    a[0] = 1;
    int i = 0;

    for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
      int v = list[ptr].v;
      if (v != parent) {
        i++;
        // Toți fiii sînt mincinoși.
        a[i] = a[i - 1] * nd[v].ways[c_liar] % MOD;

        for (int j = i - 1; j; j--) {
          // Aveam deja j mincinoși și adăugăm un sincer...
          long long gain_tt = a[j] * nd[v].ways[c_tt];
          // ... sau aveam j - 1 mincinoși și adăugăm un mincinos.
          long long gain_liar = a[j - 1] * nd[v].ways[c_liar];
          a[j] = (gain_tt + gain_liar) % MOD;
        }

        // Toți fiii sînt sinceri.
        a[0] = a[0] * nd[v].ways[c_tt] % MOD;
      }
    }
  }

  int get_sum() {
    long long result = 0;
    for (int i = 0; i <= nd[curr_node].num_children; i++) {
      result += a[i];
    }
    return result % MOD;
  }

  int get(int k) {
    if ((k >= 0) && (k <= nd[curr_node].num_children)) {
      return a[k];
    } else {
      return 0;
    }
  }
};

combo c;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
  nd[u].num_children++;
}

void read_data() {
  FILE* f = fopen("liars.in", "r");
  fscanf(f, "%d", &n);
  for (int u = 1; u <= n; u++) {
    fscanf(f, "%d", &nd[u].resp);
  }
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(f, "%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
  fclose(f);

  for (int u = 2; u <= n; u++) {
    nd[u].num_children--; // toate nodurile în afară de 1 au un părinte
  }
}

void count_dfs(int u, int parent) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      count_dfs(v, u);
    }
  }

  int r = nd[u].resp;
  // Cazul I: u este sincer.
  c.merge_children(u, parent, LIAR_P_TT, TT_P_TT);

  // Dacă și părintele este sincer, cei r mincinoși se află printre
  // fii.
  nd[u].ways[TT_P_TT] = c.get(r);

  // Dacă părintele este mincinos, ceilalți r - 1 mincinoși se află
  // printre fii.
  nd[u].ways[TT_P_LIAR] = c.get(r - 1);

  // Cazul II: u este mincinos.
  c.merge_children(u, parent, LIAR_P_LIAR, TT_P_LIAR);
  int sum = c.get_sum();

  // Dacă părintele este sincer, u trebuie să aibă orice în afară de r
  // mincinoși printre fii.
  nd[u].ways[LIAR_P_TT] = (sum + MOD - c.get(r)) % MOD;

  // Dacă și părintele este mincinos, u trebuie să aibă orice în afară de
  // r - 1 mincinoși printre fii.
  nd[u].ways[LIAR_P_LIAR] = (sum + MOD - c.get(r - 1)) % MOD;
}

// Atribuie toți fiii impuși.
int assign_forced(int u, int parent) {
  int c_liar = (nd[u].assignment == LIAR) ? LIAR_P_LIAR : LIAR_P_TT;
  int c_tt = (nd[u].assignment == LIAR) ? TT_P_LIAR : TT_P_TT;
  int num_assigned = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      if (!nd[v].ways[c_liar]) {
        nd[v].assignment = TRUTH_TELLER;
      } else if (!nd[v].ways[c_tt]) {
        nd[v].assignment = LIAR;
        num_assigned++;
      } else {
        nd[v].assignment = ANY;
      }
    }
  }

  return num_assigned;
}

void assign_optional(int u, int parent, int liars_left) {
  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (nd[v].assignment == ANY) {
      if (liars_left) {
        nd[v].assignment = LIAR;
        liars_left--;
      } else {
        nd[v].assignment = TRUTH_TELLER;
      }
    }
  }
}

// Atribuie valori fiilor lui u. Nodul lui u are deja valoare. Bug: dacă
// numărul de posibilități este 0 modulo MOD, el va fi considerat incorect ca
// fiind 0.
void assign_dfs(int u, int parent) {
  int assigned_liars = assign_forced(u, parent);
  // Și părintele poate minți (ce familie de cacao).
  assigned_liars += parent && (nd[parent].assignment == LIAR);

  int liars_left;
  if (nd[u].assignment == TRUTH_TELLER) {
    liars_left = nd[u].resp - assigned_liars;
  } else if (assigned_liars == nd[u].resp) {
    liars_left = 1; // Nu ne putem opri acum, căci u spune adevărul.
  } else {
    liars_left = 0;
  }

  assign_optional(u, parent, liars_left);

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != parent) {
      assign_dfs(v, u);
    }
  }
}

void write_answer() {
  FILE* f = fopen("liars.out", "w");
  int ans = (nd[1].ways[LIAR_P_TT] + nd[1].ways[TT_P_TT]) % MOD;
  fprintf(f, "%d\n", ans);
  for (int u = 1; u <= n; u++) {
    fprintf(f, "%d ", nd[u].assignment);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  read_data();
  count_dfs(1, 0);
  nd[1].assignment = nd[1].ways[TT_P_TT] ? TRUTH_TELLER : LIAR;
  assign_dfs(1, 0);
  write_answer();

  return 0;
}
