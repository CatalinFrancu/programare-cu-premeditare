// Ranking și unranking de combinări în ordine colexicografică.
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Vom face ranking și unranking TRIALS combinări aleatorii de N luate cîte K.
#define TRIALS 10'000'000
#define N 60
#define K 30
#define COLEX_UNRANK_STEP 3 // de ordinul a sqrt(N/K)

typedef unsigned long long u64;

typedef struct {
  u64 rank;
  int v[K];
} combination;

combination c[TRIALS];
bool seen[N]; // folosit pentru generarea combinăriilor aleatorii
u64 choose[N + 1][K + 1];

/****************************************************************
 *                     Gestiunea timpului                       *
 ****************************************************************/
long long __time1;

long long get_time() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return 1000ll * time.tv_sec + time.tv_usec / 1000;
}

void mark_time() {
  __time1 = get_time();
}

void report_time(const char* msg) {
  long long time2 = get_time();
  fprintf(stderr, "%s: %lld milisecunde\n", msg, time2 - __time1);
}

/******************************************************************
 *   Ordine colexicografică: ranking în O(k), unranking în O(n)   *
 ******************************************************************/

u64 rank_colex(int* c) {
  u64 result = 0;
  for (int i = 0; i < K; i++) {
    result += choose[(int)c[i]][i + 1];
  }
  return result;
}

void unrank_colex(u64 rank, int* witness) {
  int n = N - 1;
  for (int k = K; k; k--) {
    while (choose[n][k] > rank) {
      n--;
    }
    rank -= choose[n][k];
    assert(n == witness[k - 1]);
  }
}

// ca și unrank_colex, dar cu o optimizare (sare mai mulți pași deodată)
void unrank_colex2(u64 rank, int* witness) {
  int n = N - 1;
  for (int k = K; k; k--) {
    while (n >= COLEX_UNRANK_STEP && choose[n - COLEX_UNRANK_STEP][k] > rank) {
      n -= COLEX_UNRANK_STEP;
    }
    while (choose[n][k] > rank) {
      n--;
    }
    rank -= choose[n][k];
    assert(n == witness[k - 1]);
  }
}

int main() {
  struct timeval time;
  gettimeofday(&time, NULL);
  srand(time.tv_usec);

  // precalculează combinările
  mark_time();
  for (int n = 0; n <= N; n++) {
    for (int k = 0; k <= K; k++) {
      if (k == 0) {
        choose[n][k] = 1;
      } else if (n == 0) {
        choose[n][k] = 0;
      } else {
        choose[n][k] = choose[n - 1][k] + choose[n - 1][k - 1];
      }
    }
  }
  printf("Benchmark C(%d,%d) = %.3e, pas unranking=%d, %dM experimente\n",
         N, K, (double)choose[N][K], COLEX_UNRANK_STEP, TRIALS / 1'000'000);
  report_time("Precalcularea combinărilor");

  // generează experimentele
  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    for (int j = 0; j < K; j++) {
      int val;
      do {
        val = rand() % N;
      } while (seen[val]);
      seen[val] = true;
      c[i].v[j] = val;
    }
    std::sort(c[i].v, c[i].v + K);

    for (int j = 0; j < K; j++) {
      seen[(int)c[i].v[j]] = false;
    }
  }
  report_time("Generarea experimentelor");
  printf("--\n");

  // benchmarking
  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    c[i].rank = rank_colex(c[i].v);
  }
  report_time("Ranking colexicografic");

  mark_time();
  for (int i = 0; i < TRIALS; i++) {
    unrank_colex(c[i].rank, c[i].v);
  }
  report_time("Unranking colexicografic");
  mark_time();

  for (int i = 0; i < TRIALS; i++) {
    unrank_colex2(c[i].rank, c[i].v);
  }
  report_time("Unranking colexicografic cu optimizare");
  return 0;
}
