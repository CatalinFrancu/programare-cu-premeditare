// Notă: _pdep_u32 necesită argumentul -march=native la compilare.
//
// Ranking și unranking de permutări prin mai multe metode. Nu sînt ieșite din
// comun. Ne interesează să fie ușor de codat.
#include <assert.h>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Vom face ranking și unranking pe NUM_PASSES * NUM_PERMS permutări aleatorii
// de mărime n.
const int NUM_PERMS = 1'000;
const int NUM_PASSES = 10'000;
const int N = 20;
const int MAX_P2 = 16; // pentru fenwick_bin_search()

typedef unsigned long long u64;

typedef struct {
  u64 rank, witness;
  int v[N];
} permutation;

permutation p[NUM_PERMS], q[NUM_PERMS];
u64 fact[N];
char kth[1 << N][N]; // kth[x][k] = al k-lea bit zero în x (0-based)

void generate_permutations() {
  for (int i = 0; i < NUM_PERMS; i++) {
    for (int j = 0; j < N; j++) {
      int k = rand() % (j + 1);
      p[i].v[j] = p[i].v[k];
      p[i].v[k] = j;
    }
  }
}

void precompute_factorials() {
  fact[0] = 1;
  for (int i = 1; i < N; i++) {
    fact[i] = fact[i - 1] * i;
  }
}

void precompute_kth() {
  for (int i = 0; i < (1 << N); i++) {
    int cnt = 0;
    for (int k = 0; k < N; k++) {
      if ((i & (1 << k)) == 0) {
        kth[i][cnt++] = k;
      }
    }
    // Pozițiile de la cnt la N sînt irelevante pentru că nu există destui
    // biți setați. Fie le atribuim o valoare specială, fie ne asigurăm că
    // codul nu citeșțe acele poziții.
  }
}

void verify_rank() {
  for (int i = 0; i < NUM_PERMS; i++) {
    assert(p[i].rank == p[i].witness);
  }
}

void verify_unrank() {
  for (int i = 0; i < NUM_PERMS; i++) {
    for (int j = 0; j < N; j++) {
      assert(p[i].v[j] == q[i].v[j]);
    }
  }
}

/****************************************************************
 *                     Gestiunea timpului                       *
 ****************************************************************/
long long t0;

long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  t0 = get_time();
}

void report_time(const char* msg) {
  int millis = get_time() - t0;
  int ops = NUM_PASSES * NUM_PERMS;

  printf("%s: %d ms pentru %d operații (%d Mops/sec)\n",
         msg, millis, ops, ops / millis / 1000);
}

/****************************************************************
 *                       Metoda naivă                           *
 ****************************************************************/

bool used[N];

void rank_naive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      u64 rank = 0;
      for (int i = 0; i < N; i++) {
        // Numără elementele mai mici decît p[i].
        int cnt = 0;
        for (int j = 0; j < i; j++) {
          cnt += (p[k].v[j] < p[k].v[i]);
        }
        rank += fact[N - 1 - i] * (p[k].v[i] - cnt);
      }
      p[k].rank = p[k].witness = rank;
    }
  }
}

void unrank_naive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      for (int i = 0; i < N; i++) {
        used[i] = false;
      }
      u64 rank = p[k].rank;
      for (int i = 0; i < N; i++) {
        int quot = rank / fact[N - 1 - i];
        rank %= fact[N - 1 - i];
        // Caută a quot-a valoare nefolosită.
        int j = 0;
        while (quot || used[j]) {
          quot -= !used[j++];
        }
        q[k].v[i] = j;
        used[j] = true;
      }
    }
  }
}

/****************************************************************
 *                       Arbori Fenwick                         *
 ****************************************************************/

// Reminder: AIB-urile sînt indexate de la 1.
int fen[N + 1];

void fenwick_clear() {
  for (int i = 1; i <= N; i++) {
    fen[i] = 0;
  }
}

// Pune toate valorile pe 1 și apoi construiește AIB-ul.
// https://stackoverflow.com/a/31070683/6022817
void fenwick_set_1() {
  for (int i = 1; i <= N; i++) {
    fen[i] = 1;
  }
  for (int i = 1; i <= N; i++) {
    int j = i + (i & -i);
    if (j <= N) {
      fen[j] += fen[i];
    }
  }
}

void fenwick_add(int pos, int val) {
  do {
    fen[pos] += val;
    pos += pos & -pos;
  } while (pos <= N);
}

int fenwick_sum(int pos) {
  int s = 0;
  while (pos) {
    s += fen[pos];
    pos &= pos - 1;
  }
  return s;
}

// Returnează cea mai mare poziție pentru care fen[pos] < val.
int fenwick_bin_search(int val) {
  int pos = 0;

  for (int interval = MAX_P2; interval; interval >>= 1) {
    if ((pos + interval <= N) && (fen[pos + interval] < val)) {
      val -= fen[pos + interval];
      pos += interval;
    }
  }

  return pos;
}

void rank_fenwick() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      fenwick_clear();
      u64 rank = 0;
      for (int i = 0; i < N; i++) {
        // Numără elementele mai mici decît p[i] -- ca în versiunea naivă.
        int cnt = fenwick_sum(p[k].v[i] + 1);
        rank += fact[N - 1 - i] * (p[k].v[i] - cnt);
        fenwick_add(p[k].v[i] + 1, 1);
      }
      p[k].rank = rank;
    }
  }
}

void unrank_fenwick() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      u64 rank = p[k].rank;
      // Valorile 1 din AIB indică elemente încă folosibile.
      fenwick_set_1();
      for (int i = 0; i < N; i++) {
        int quot = rank / fact[N - 1 - i];
        rank %= fact[N - 1 - i];
        // Caută a quot-a valoare folosibilă.
        int j = fenwick_bin_search(quot + 1);
        q[k].v[i] = j;
        fenwick_add(j + 1, -1);
      }
    }
  }
}

/****************************************************************
 *                           Popcount                           *
 ****************************************************************/

void rank_popcount() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      u64 rank = 0;
      int mask = 0;
      for (int i = 0; i < N; i++) {
        // Numără biții setați pe pozițiile [0...p[i]).
        int cnt = __builtin_popcount(mask & ((1 << p[k].v[i]) - 1));
        rank += fact[N - 1 - i] * (p[k].v[i] - cnt);
        mask |= (1 << p[k].v[i]);
      }
      p[k].rank = rank;
    }
  }
}

void unrank_popcount() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      u64 rank = p[k].rank;
      int mask = 0;
      for (int i = 0; i < N; i++) {
        int quot = rank / fact[N - 1 - i];
        rank %= fact[N - 1 - i];
        // Caută a quot-a valoare nefolosită.
        int j = kth[mask][quot];
        q[k].v[i] = j;
        mask |= (1 << j);
      }
    }
  }
}

void unrank_popcount2() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int k = 0; k < NUM_PERMS; k++) {
      u64 rank = p[k].rank;
      int mask = 0;
      for (int i = 0; i < N; i++) {
        int quot = rank / fact[N - 1 - i];
        rank %= fact[N - 1 - i];

        // Caută a quot-a valoare nefolosită.
        int zero_bit_mask = _pdep_u32(1 << quot, ~mask);
        int j = _tzcnt_u32(zero_bit_mask);

        q[k].v[i] = j;
        mask |= (1 << j);
      }
    }
  }
}

int main() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  mark_time();
  precompute_factorials();
  precompute_kth();
  generate_permutations();
  rank_naive();
  report_time("inițializare");
  printf("--\n");

  mark_time();
  rank_naive();
  report_time("rank naiv");
  verify_rank();

  mark_time();
  rank_fenwick();
  report_time("rank cu AIB");
  verify_rank();

  mark_time();
  rank_popcount();
  report_time("rank cu popcount");
  verify_rank();
  printf("--\n");

  mark_time();
  unrank_naive();
  report_time("unrank naiv");
  verify_unrank();

  mark_time();
  unrank_fenwick();
  report_time("unrank cu AIB");
  verify_unrank();

  mark_time();
  unrank_popcount();
  report_time("unrank cu popcount și tabel");
  verify_unrank();

  mark_time();
  unrank_popcount2();
  report_time("unrank cu popcount și pdep");
  verify_unrank();

  return 0;
}
