// Complexitate: O(n log n) datorită sortării, O(n log* n) în rest.
//
// v3: Citire rapidă. Problema este acum dominată de std::sort, care ocupă cam
// 65% din timpul de rulare.
#include <algorithm>
#include <ctype.h>
#include <stdio.h>

const int MAX_QUERIES = 200'000;
const int MAX_EVENTS = 600'000; // trambuline + 2 * interogări
const int T_QUERY_START = 1;
const int T_QUERY_END = 2;
const int T_TRAMPOLINE = 3;
const int NONE = -1;

struct event {
  int r, c;
  int index; // relevant doar pentru interogări
  unsigned char type;
};

// O structură de mulțimi disjuncte pentru indici de interogări. Reține și
// rîndul curent normalizat pentru fiecare indice.
struct disjoint_set_forest {
  int p[MAX_QUERIES];
  int row[MAX_QUERIES]; // relevant doar pentru rădăcini
  int n;

  void init(int n) {
    for (int u = 0; u < n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  int get_row(int u) {
    return row[find(u)];
  }

  // Unifică v cu u întotdeauna.
  void unite(int u, int v) {
    p[find(v)] = find(u);
  }
};

event e[MAX_EVENTS];
int orig_r[MAX_EVENTS];
bool answer[MAX_QUERIES];
disjoint_set_forest dsf;

// at_row[i] = indicile mulțimii de pe rîndul normalizat i
int at_row[MAX_EVENTS];

int n; // numărul de evenimente
int num_queries;

struct fast_input {
  static const int BUF_SIZE = 4096;
  char buf[BUF_SIZE + 1];
  int pos = BUF_SIZE;

  char get_char() {
    if (pos == BUF_SIZE) {
      int read = fread(buf, 1, BUF_SIZE, stdin);
      buf[read] = '\0';
      pos = 0;
    }
    return buf[pos++];
  }

  int read_int() {
    int result = 0;
    char c;

    do {
      c = get_char();
    } while (isspace(c));

    do {
      result = 10 * result + c - '0';
      c = get_char();
    } while (isdigit(c));

    return result;
  }
};

void read_data() {
  fast_input f;

  f.read_int();
  f.read_int(); // ignoră r și c
  n = f.read_int();

  for (int i = 0; i < n; i++) {
    e[i] = { f.read_int(), f.read_int(), 0, T_TRAMPOLINE };
  }

  num_queries = f.read_int();
  for (int i = 0; i < num_queries; i++) {
    int r1 = f.read_int();
    int c1 = f.read_int();
    int r2 = f.read_int();
    int c2 = f.read_int();
    // Ignoră interogările care se termină înainte să înceapă. Vezi
    // comentariul pentru process_query_end().
    if ((r2 >= r1) && (c2 >= c1)) {
      e[n++] = { r1, c1, i, T_QUERY_START };
      e[n++] = { r2, c2, i, T_QUERY_END };
    }
  }
}

void normalize_rows() {
  std::sort(e, e + n, [](event a, event b) {
    return (a.r < b.r);
  });

  int ptr = -1;

  for (int i = 0; i < n; i++) {
    if ((ptr < 0) || (e[i].r != orig_r[ptr])) {
      orig_r[++ptr] = e[i].r;
    }
    e[i].r = ptr;
  }
}

// Sortează crescător după coloană, apoi crescător după rînd, apoi după tipul
// evenimentului (pentru că se poate să începem o interogare, apoi să coborîm
// imediat).
void sort_events() {
  std::sort(e, e + n, [](event a, event b) {
    return
      (a.c < b.c) ||
      ((a.c == b.c) && (a.r < b.r)) ||
      ((a.c == b.c) && (a.r == b.r) && (a.type < b.type));
  });
}

void process_query_start(event& e) {
  if (at_row[e.r] == NONE) {
    // e este singura interogare de pe acest rînd.
    dsf.row[e.index] = e.r;
    at_row[e.r] = e.index;
  } else {
    dsf.unite(at_row[e.r], e.index);
  }
}

// Interogarea este fezabilă dacă a reușit să călătorească la e.r *sau mai
// jos* (pentru că ne-am fi putut opri mai devreme dacă doream). Totuși,
// aceasta nu distinge cazul în care r2 > r1 încă de la început, care nu este
// fezabil. Pe acela îl filtrăm la intrare.
void process_query_end(event& e) {
  // Care sînt coordonatele prezente?
  int row = dsf.get_row(e.index);
  answer[e.index] = (row >= e.r);
}

// Toate interogările de pe acest rînd coboară la următorul, dar numai dacă
// coordonatele originale diferă prin exact 1.
//
// Notă: Aceasta înseamnă că niciodată nu coborîm la un rînd unde nu există
// nici trambuline nici interogări. Acel rînd nici nu există printre
// coordonatele normalizate. Aceasta este corect; nu avem niciun motiv să
// facem asta.
void process_trampoline(event& e) {
  if ((at_row[e.r] != NONE) && (orig_r[e.r + 1] == orig_r[e.r] + 1)) {
    if (at_row[e.r + 1] == NONE) {
      // Nu există interogări pe rîndul e.r + 1. Mută mulțimea acolo.
      dsf.row[at_row[e.r]]++;
      at_row[e.r + 1] = at_row[e.r];
    } else {
      // Unifică mulțimile.
      dsf.unite(at_row[e.r + 1], at_row[e.r]);
    }
    // Nu mai avem interogări pe acest rînd (pot apărea altele mai tîrziu).
    at_row[e.r] = NONE;
  }
}

void scan() {
  // Inițializează linia de baleiere.
  for (int i = 0; i < n; i++) {
    at_row[i] = NONE;
  }
  dsf.init(num_queries);

  for (int i = 0; i < n; i++) {
    switch (e[i].type) {
      case T_QUERY_START:
        process_query_start(e[i]);
        break;
      case T_QUERY_END:
        process_query_end(e[i]);
        break;
      case T_TRAMPOLINE:
        process_trampoline(e[i]);
        break;
    }
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    printf(answer[i] ? "Yes\n" : "No\n");
  }
}

int main() {
  read_data();
  normalize_rows();
  sort_events();
  scan();
  write_answers();

  return 0;
}
