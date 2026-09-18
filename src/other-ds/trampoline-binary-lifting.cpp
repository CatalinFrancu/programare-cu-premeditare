// Complexitate: O((n + q) log n).
#include <algorithm>
#include <map>
#include <stdio.h>

const int MAX_TRAMPOLINES = 200'000;
const int MAX_QUERIES = 200'000;
const int MAX_LOG = 18;

struct trampoline {
  int r, c;

  // cîte rînduri putem coborî de la această trambulină
  int len;

  // parent[i] = trambulina cu 2^i rînduri sub noi, spre dreapta
  int parent[MAX_LOG + 1];
};

struct query {
  int r1, c1, r2, c2;
  int orig_index;
};

trampoline t[MAX_TRAMPOLINES + 1];
std::map<int,int> active_t;
query q[MAX_QUERIES];
bool answer[MAX_QUERIES];
int n, num_queries;

void read_data() {
  int r, c; // ignorate
  scanf("%d %d %d\n", &r, &c, &n);

  for (int i = 1; i <= n; i++) {
    scanf("%d %d", &t[i].r, &t[i].c);
  }

  scanf("%d", &num_queries);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d %d %d", &q[i].r1, &q[i].c1, &q[i].r2, &q[i].c2);
    q[i].orig_index = i;
  }
}

// Sortează după col descrescător, apoi după row descrescător.
void sort_trampolines() {
  std::sort(t + 1, t + n + 1, [](trampoline a, trampoline b) {
    return (a.c > b.c) || ((a.c == b.c) && (a.r > b.r));
  });
}

// Idem.
void sort_queries() {
  std::sort(q, q + num_queries, [](query a, query b) {
    return (a.c1 > b.c1) || ((a.c1 == b.c1) && (a.r1 > b.r1));
  });
}

void process_trampoline(int i) {
  auto it = active_t.find(t[i].r + 1);
  if (it == active_t.end()) {
    // Nu există altă trambulină verde la care să sărim. Încă putem coborî un rînd folosind
    // această trambulină.
    t[i].len = 1;
  } else {
    int j = it->second;
    t[i].len = 1 + t[j].len;
    t[i].parent[0] = j;
    for (int k = 1; k <= MAX_LOG; k++) {
      t[i].parent[k] = t[t[i].parent[k - 1]].parent[k - 1];
    }
  }
  active_t[t[i].r] = i;
}

// Călătorește delta rînduri în jos de la t[i].
int descend(int delta, int i) {
  while (delta) {
    int bit = __builtin_ctz(delta);
    i = t[i].parent[bit];
    delta &= delta - 1;
  }
  return i;
}

bool process_query(query& q) {
  if (q.r1 == q.r2) {
    // Putem merge la dreapta, dar nu și la stînga, pe rîndul curent.
    return (q.c1 <= q.c2);
  }

  if (q.r1 > q.r2) {
    // Nu putem urca.
    return false;
  }

  auto it = active_t.find(q.r1);
  if (it == active_t.end()) {
    return false; // Nu există trambulină pe rîndul nostru.
  }

  int i = it->second;
  int to_descend = q.r2 - q.r1;
  if (t[i].len < to_descend) {
    // Există un șir de trambuline, dar nu coboară suficient.
    return false;
  }

  // Coboară doar pînă la rîndul r2-1. Ne pasă de coloana acelei trambuline,
  // pentru că ne va permite să coborîm la r2.
  int j = descend(to_descend - 1, i);
  return (t[j].c <= q.c2); // Am ajuns pe rîndul r2 înainte sau după c2?
}

void scan() {
  int j = 0; // indicele interogării
  for (int i = 1; i <= n; i++) {
    process_trampoline(i);
    // Acum putem răspunde la toate interogările care încep la dreapta sau sub
    // t[i + 1].
    while ((j < num_queries) &&
           ((i == n) ||
            (q[j].c1 > t[i + 1].c) ||
            ((q[j].c1 == t[i + 1].c) && (q[j].r1 > t[i + 1].r)))) {
      answer[q[j].orig_index] = process_query(q[j]);
      j++;
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
  sort_trampolines();
  sort_queries();
  scan();
  write_answers();

  return 0;
}
