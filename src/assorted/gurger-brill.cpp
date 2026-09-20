// Complexitate: O(N log N) și exact 4 ⌈log N⌉ întîlniri. Pentru a trece unele
// teste adversariale, trece totul printr-o permutare aleatorie.
#include <algorithm>
#include <random>
#include "gurgerbrill.h"

const int MAX_N = 100'000;

// Vezi comentariile pentru datele din BIT_CONFIG.
const int BIT_CONFIG[2][3][3] = {
  { { 3, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 } },
  { { 0, 0, 3 }, { 0, 3, 1 }, { 3, 1, 0 } },
};

std::vector<int> perm, question, resp, sol;
int hp[MAX_N], hp1[MAX_N + 1]; // permutare ascunsă și inversa ei
unsigned char left1[MAX_N], left2[MAX_N];
bool seen[MAX_N];
int masks[MAX_N][2];
int n;

void gen_hidden_perm() {
  std::mt19937 rng(0);
  for (int i = 0; i < n; i++) {
    hp[i] = i + 1;
  }
  std::shuffle(hp, hp + n, rng);
  for (int i = 0; i < n; i++) {
    hp1[hp[i]] = i;
  }
}

// Trece permutarea prin cea ascunsă. Apelează query(). Traduce
// răspunsul înapoi în notația noastră.
void wrap_meeting() {
  for (int i = 0; i < n; i++) {
    question[i] = hp[perm[i]];
  }
  resp = query(question);
  for (int i = 0; i < (int)resp.size(); i++) {
    resp[i] = hp1[resp[i]];
  }
}

// Separă valorile [0...n) după valoarea bitului b. Returnează numărul de
// valori din stînga.
int partition_by_bit(int b) {
  int left = 0, right = n;
  for (int i = 0; i < n; i++) {
    if (i & (1 << b)) {
      perm[--right] = i;
    } else {
      perm[left++] = i;
    }
  }

  return left;
}

// Face două interogări, pentru perm și pentru perm răsturnat. Scrie în dest
// numărul de vecini din stînga ai fiecărei persoane.
void two_way_query(unsigned char* dest) {
  for (int i = 0; i < n; i++) {
    dest[i] = 1;
  }
  wrap_meeting();
  for (int early_person: resp) {
    dest[early_person] = 0;
  }

  std::reverse(perm.begin(), perm.end());

  wrap_meeting();
  for (int early_person: resp) {
    dest[early_person] = 2;
  }

  std::reverse(perm.begin(), perm.end());

}

void deduce_kth_bit(int b) {
  for (int i = 0; i < n; i++) {
    int my_side = (i >> b) & 1;
    int mask = BIT_CONFIG[my_side][left1[i]][left2[i]];
    masks[i][0] |= (mask & 1) << b;
    masks[i][1] |= (mask >> 1) << b;
  }
}

int get_num_bits(int n) {
  return 32 - __builtin_clz(n - 1);
}

// Face cîte două perechi de interogări, pe baza cărora află biții vecinilor
// fiecărei persoane.
void build_neighbor_bits() {
  int num_bits = get_num_bits(n);
  for (int b = 0; b < num_bits; b++) {
    int sep = partition_by_bit(b);
    two_way_query(left1);
    std::reverse(perm.begin(), perm.begin() + sep);
    std::reverse(perm.begin() + sep, perm.end());
    two_way_query(left2);
    deduce_kth_bit(b);
  }
}

// Caută persoana cu numărul minim de biți variabili între cei doi vecini.
int find_most_constrained_person() {
  int min_pop = MAX_N, min_person = MAX_N;
  for (int i = 0; i < n; i++) {
    int x = masks[i][0] ^ masks[i][1];
    int pc = __builtin_popcount(x);
    if (pc < min_pop) {
      min_pop = pc;
      min_person = i;
    }
  }

  return min_person;
}

bool is_submask(int submask, int mask) {
  return (mask & submask) == submask;
}

// Returnează true dacă și numai dacă masca mask respectă cerințele pe biți
// pentru vecinii lui person.
bool fits_in(int mask, int person) {
  return
    is_submask(mask, masks[person][0] | masks[person][1]) &&
    is_submask(~mask, ~masks[person][0] | ~masks[person][1]);
}

// Dîndu-se un vecin al lui person (care îi respectă cerințele pe biți), îl
// returnează pe celălalt.
int other_neighbor(int person, int first_neighbor) {
  return masks[person][0] ^ masks[person][1] ^ first_neighbor;
}

// Returnează true dacă și numai dacă, începînd cu persoanele A și B, restul
// mesei poate fi completat.
bool complete_circle(int a, int b) {
  for (int i = 0; i < n; i++) {
    seen[i] = false;
  }
  seen[a] = true;
  sol[0] = a;

  int num_seen = 1;
  while ((b < n) && !seen[b]) {
    seen[b] = true;
    sol[num_seen++] = b;
    int tmp = other_neighbor(b, a);
    a = b;
    b = tmp;
  }

  return (num_seen == n);
}

// Caută primul vecin îl lui first astfel încît să putem completa masa.
void try_all_neighbors(int first) {
  sol.resize(n);
  int i = 0;
  while ((i == first) ||
         !fits_in(i, first) ||
         !complete_circle(first, i)) {
    i++;
  }
}

// Trece și soluția prin permutarea ascunsă.
void wrap_answer() {
  for (int i = 0; i < n; i++) {
    sol[i] = hp[sol[i]];
  }
}

std::vector<int> find_places(int N) {
  n = N;
  perm.resize(n);
  question.resize(n);
  gen_hidden_perm();
  build_neighbor_bits();
  int first = find_most_constrained_person();
  try_all_neighbors(first);
  wrap_answer();
  return sol;
}
