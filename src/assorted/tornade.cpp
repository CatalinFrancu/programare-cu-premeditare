// Complexitate: O(n).
#include <stdio.h>

const int MAX_N = 1'000'000;
const int T_LOCAL_MIN = 0;
const int T_LOCAL_MAX = 1;
const int T_MIDDLE = 2;

// O serie de elemente egale. Contract:
//
// * Lungimea reală a unei serii este cnt + global_min_cnt pentru minime
//   locale, cnt + global_max_cnt pentru maxime locale și cnt simplu pentru
//   valori intermediare.
//
// * Un optim local aparține listei de minime sau de maxime corespunzătoare
//   valorii cnt. Valorile intermediare nu fac parte din nicio listă pe
//   lungimi.
struct run {
  int val, cnt;
  char type;
  int gprev, gnext; // pointeri în lista înlănțuită globală
  int sprev, snext; // pointeri în lista înlănțuită de optime locale (dacă există)
  int self;         // indicele propriu în r[], pentru înlănțuire

  void add_to_size_list(int& head);
  void classify();
  void remove_from_size_list();
  void declassify();
  void decrease_cnt();
  void remove_from_global_list();
  void merge_with_next();
};

run r[MAX_N + 1]; // indexat de la 1; 0 înseamnă nul
int global_min_cnt, global_max_cnt;
int ghead, gtail;
int min_head[MAX_N], max_head[MAX_N];
int n;

int mod(int x) {
  return (x < 0) ? (x + n) : x;
}

void run::add_to_size_list(int& head) {
  if (head) {
    r[head].sprev = self;
    snext = head;
  }
  head = self;
}

void run::classify() {
  if ((!gprev || (val < r[gprev].val)) &&
      (!gnext || (val < r[gnext].val))) {
    type = T_LOCAL_MIN;
    cnt -= global_min_cnt;
    this->add_to_size_list(min_head[mod(cnt)]);
  } else if ((!gprev || (val > r[gprev].val)) &&
             (!gnext || (val > r[gnext].val))) {
    type = T_LOCAL_MAX;
    cnt -= global_max_cnt;
    this->add_to_size_list(max_head[mod(cnt)]);
  } else {
    type = T_MIDDLE;
  }
}

void run::remove_from_size_list() {
  if (type == T_MIDDLE) {
    return;
  }

  int m = mod(cnt);
  int& head = (type == T_LOCAL_MIN) ? min_head[m] : max_head[m];

  if (sprev) {
    r[sprev].snext = snext;
  } else {
    head = snext;
  }

  if (snext) {
    r[snext].sprev = sprev;
  }

  sprev = snext = 0;
}

void run::declassify() {
  remove_from_size_list();
  if (type == T_LOCAL_MIN) {
    cnt += global_min_cnt;
  } else if (type == T_LOCAL_MAX) {
    cnt += global_max_cnt;
  }
  type = T_MIDDLE;
}

// Cînd cnt se schimbă, seria migrează la o nouă listă pe lungimi.
void run::decrease_cnt() {
  declassify();
  cnt--;
  classify();
}

void run::remove_from_global_list() {
  if (gprev) {
    r[gprev].gnext = gnext;
  } else {
    ghead = gnext;
  }

  if (gnext) {
    r[gnext].gprev = gprev;
  } else {
    gtail = gprev;
  }
}

void run::merge_with_next() {
  cnt += r[gnext].cnt;
  r[gnext].remove_from_global_list();
}

void add_run(int val, int cnt) {
  static int ptr = 1;

  gtail = ptr;
  r[ptr].val = val;
  r[ptr].cnt = cnt;
  r[ptr].self = ptr;
  if (ptr > 1) {
    r[ptr - 1].gnext = ptr;
    r[ptr].gprev = ptr - 1;
  }
  ptr++;
}

void read_array(FILE* f) {
  int prev, val, cnt = 1;

  fscanf(f, "%d %d", &n, &prev);
  ghead = 1;
  for (int i = 1; i < n; i++) {
    fscanf(f, "%d ", &val);
    if (val == prev) {
      cnt++;
    } else {
      add_run(prev, cnt);
      prev = val;
      cnt = 1;
    }
  }
  add_run(prev, cnt);
}

void init_local_optima() {
  for (int ptr = ghead; ptr; ptr = r[ptr].gnext) {
    r[ptr].classify();
  }
}

void delete_local_optimum(int i) {
  int prev = r[i].gprev;
  int next = r[i].gnext;

  r[i].remove_from_global_list();

  if (prev) {
    r[prev].declassify();
  }
  if (next) {
    r[next].declassify();
  }

  if (prev && next && (r[prev].val == r[next].val)) {
    r[prev].merge_with_next();
    r[prev].classify();
  } else {
    if (prev) {
      r[prev].classify();
    }
    if (next) {
      r[next].classify();
    }
  }
}

void delete_zero_size_list(int& head) {
  for (int ptr = head; ptr; ptr = r[ptr].snext) {
    delete_local_optimum(ptr);
  }
  head = 0;
}

void operation(int& best_cnt, int& worst_cnt, int type, int* head) {
  best_cnt++;

  if (r[ghead].type == type) {
    r[ghead].decrease_cnt();
  }
  if (r[gtail].type == type) {
    r[gtail].decrease_cnt();
  }

  worst_cnt--;
  int pos = mod(-worst_cnt);
  delete_zero_size_list(head[pos]);
}

// Procesează operații pînă cînd ajungem la o singură serie cu elemente
// egale. Poate fi nevoie sau nu să procesăm toate operațiile M și m. Astfel
// evităm cazul particular în care ultima serie rămasă este simultan maxim și
// minim local.
void build_pyramid(FILE* f) {
  while (r[ghead].gnext) {
    if (fgetc(f) == 'm') {
      operation(global_min_cnt, global_max_cnt, T_LOCAL_MIN, max_head);
    } else {
      operation(global_max_cnt, global_min_cnt, T_LOCAL_MAX, min_head);
    }
  }
}

int main() {
  FILE* fin = fopen("tornade.in", "r");
  read_array(fin);
  init_local_optima();
  build_pyramid(fin);
  fclose(fin);

  FILE* fout = fopen("tornade.out", "w");
  fprintf(fout, "%d\n", r[ghead].val);
  fclose(fout);

  return 0;
}
