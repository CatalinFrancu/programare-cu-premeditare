// Implementează criteriul de translație <https://minos.tessera.li/tiling>
#include <stdio.h>
#include <unordered_set>

const int MAX_SQUARES = 2'300;
const int MAX_EDGES = 2 * MAX_SQUARES + 2; // cazul denegerat, n×1
const int MOD = 999'999'667;
const int INV_4_MOD = MOD / 4 + 1;

// Implementez propria pereche pentru că „r” și „c” înseamnă „row” și
// „column”, pe cînd „first” și „second” nu înseamnă nimic.
struct pair {
  int r, c;

  bool operator==(const pair& other) const {
    return (r == other.r) && (c == other.c);
  }

  struct hash {
    std::size_t operator()(const pair& p) const {
      return p.r * MAX_SQUARES + p.c;
    }
  };
};

struct edge {
  pair start, end;

  bool touches(pair p) {
    return (start == p) || (end == p);
  }

  void swap(edge& other) {
    edge tmp = *this;
    *this = other;
    other = tmp;
  }

  void orient_from(pair src) {
    if (start != src) {
      pair tmp = start;
      start = end;
      end = tmp;
    }
  }

  int get_dir() {
    // Este important ca left ^ right = 1 și up ^ down = 1. Mai jos folosim
    // acest artificiu ca să parcurgem conturul în ambele sensuri fără cod
    // suplimentar.
    if (end.r == start.r - 1) {
      return 0;
    } else if (end.r == start.r + 1) {
      return 1;
    } else if (end.c == start.c - 1) {
      return 2;
    } else {
      return 3;
    }
  }
};

int pow4[2 * MAX_EDGES + 1], inv_pow4[2 * MAX_EDGES + 1];

// Un șir 1-based de valori în baza 4, cu hash-uri parțiale.
struct hashed_string {
  char s[MAX_EDGES];
  int h[MAX_EDGES];
  int len;

  void clear() {
    len = 0;
  }

  void push(int c) {
    s[len] = c;
    h[len] = len
      ? ((long long)pow4[len] * c + h[len - 1]) % MOD
      : c;
    len++;
  }

  // inclusiv
  int substring_hash(int lo, int hi) {
    int before = lo ? h[lo - 1] : 0;
    return (long long)(MOD + h[hi] - before) * inv_pow4[lo] % MOD;
  }
};

// datele de intrare
std::unordered_set<pair, pair::hash> squares;

// colecția de muchii pe care ulterior o vom sorta
edge e[MAX_EDGES];
int num_edges, half_perim;

// conturul rezultat
int dir[MAX_EDGES];

// parcurgerile în cele două sensuri
hashed_string s1, s2;

// lungimile prefixelor din primul contur care sînt sufixe în al doilea
int match_left[MAX_EDGES], num_match_left;

// lungimile sufixelor din primul contur care sînt prefixe în al doilea
int match_right[MAX_EDGES], num_match_right;

void precompute_pow4() {
  pow4[0] = inv_pow4[0] = 1;
  for (int i = 1; i <= 2 * MAX_EDGES; i++) {
    pow4[i] = 4ll * pow4[i - 1] % MOD;
    inv_pow4[i] = (long long)INV_4_MOD * inv_pow4[i - 1] % MOD;
  }
}

void read_data() {
  int num_squares;
  scanf("%d", &num_squares);

  squares.clear();
  while (num_squares--) {
    pair sq;
    scanf("%d %d", &sq.r, &sq.c);
    squares.insert(sq);
  }
}

// Extrage muchiile din pătratele de la intrare.
void collect_edges() {
  num_edges = 0;
  for (pair sq: squares) {
    if (!squares.contains({sq.r - 1, sq.c})) {
      e[num_edges++] = {{sq.r - 1, sq.c - 1}, {sq.r - 1, sq.c}};
    }
    if (!squares.contains({sq.r, sq.c + 1})) {
      e[num_edges++] = {{sq.r - 1, sq.c}, {sq.r, sq.c}};
    }
    if (!squares.contains({sq.r + 1, sq.c})) {
      e[num_edges++] = {{sq.r, sq.c - 1}, {sq.r, sq.c}};
    }
    if (!squares.contains({sq.r, sq.c - 1})) {
      e[num_edges++] = {{sq.r - 1, sq.c - 1}, {sq.r, sq.c -1}};
    }
  }
  half_perim = num_edges / 2;
}

// Ordonează muchiile. Pornind cu una la întîmplare, o caută pe următoarea
// care se potrivește cap la cap cu prima etc. O(n^2) pentru că motive.
// Notează direcțiile în dir[] (coordonatele absolute nu ne mai interesează
// ulterior).
void trace_contour() {
  dir[0] = e[0].get_dir();

  int i = 1;
  do {
    // Găsește o continuare pentru e[i - 1].
    int j = i;
    while (!e[j].touches(e[i - 1].end)) {
      j++;
    }
    e[i].swap(e[j]);
    e[i].orient_from(e[i - 1].end);
    dir[i] = e[i].get_dir();
    i++;
  } while (e[i - 1].end != e[0].start);

  // Caz netratat: dacă i < num_edges, atunci nu am înnodat toate muchiile. De
  // ce? Pentru că figura are găuri.
}

// Construiește șirurile în baza 4 cu hashuri parțiale. Al doilea este dublat,
// deoarece cu o singură copie jumătatea pe care o testăm ar putea fi
// înfășurată peste finalul șirului.
void build_strings() {
  s1.clear();
  for (int i = 0; i < num_edges; i++) {
    s1.push(dir[i]);
  }

  s2.clear();
  for (int times = 0; times < 2; times++) {
    for (int i = num_edges - 1; i >= 0; i--) {
      s2.push(dir[i] ^ 1);
    }
  }
}

// Colectează prefixele din primul contur care sînt sufixe în al doilea și
// vițăvercea.
void collect_matches(int start1, int start2) {
  num_match_left = num_match_right = 0;

  int end1 = start1 + half_perim - 1;
  int end2 = start2 + half_perim - 1;

  for (int i = 1; i < half_perim; i++) {
    int h1 = s1.substring_hash(start1, start1 + i - 1);
    int h2 = s2.substring_hash(end2 - i + 1, end2);
    if (h1 == h2) {
      match_left[num_match_left++] = i;
    }

    h1 = s1.substring_hash(end1 - i + 1, end1);
    h2 = s2.substring_hash(start2, start2 + i - 1);
    if (h1 == h2) {
      match_right[num_match_right++] = i;
    }
  }
}

// Pentru două jumătăți care încep la start1 pe primul contur, respectiv la
// start2 pe al doilea, testează dacă eliminînd l caractere din stînga și r
// din dreapta din primul contur (și invers din al doilea), bucățile care
// rămîn sînt egale.
bool test_middle(int start1, int start2, int l, int r) {
  if (l + r > half_perim) {
    return false;
  }

  int end1 = start1 + half_perim - 1;
  int end2 = start2 + half_perim - 1;

  int h1 = s1.substring_hash(start1 + l, end1 - r);
  int h2 = s2.substring_hash(start2 + r, end2 - l);

  return (h1 == h2);
}

// Testează toate posibilitățile de a elimina prefixe și sufixe colectate
// anterior. O(n^2), dar posibilitățile sînt foarte puține.
bool halves_match(int start1, int start2) {
  collect_matches(start1, start2);

  for (int i = 0; i < num_match_left; i++) {
    for (int j = 0; j < num_match_right; j++) {
      int l = match_left[i];
      int r = match_right[j];
      if (test_middle(start1, start2, l, r)) {
        return true;
      }
    }
  }

  return false;
}

// Dance for me, dance for me, dance for me, oh-oh-oh.
bool translation_criterion() {
  collect_edges();
  trace_contour();
  build_strings();

  for (int start1 = 0; start1 < num_edges; start1++) {
    int start2 = num_edges - start1;
    if (halves_match(start1, start2)) {
      return true;
    }
  }

  return false;
}

int main() {
  precompute_pow4();

  int num_tests;
  scanf("%d", &num_tests);

  while (num_tests--) {
    read_data();
    int answer = translation_criterion();
    puts(answer ? "Yes" : "No");
  }

  return 0;
}
