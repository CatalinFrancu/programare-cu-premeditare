#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int N = 500'000;
const int MAX_LEVELS = 21;
const int INF = 2'000'000'000;

struct node {
  int val;
  int height;
  int next[MAX_LEVELS];
  int dist[MAX_LEVELS];
};

struct skip_list {
  node a[N + 2];
  int prev[MAX_LEVELS], prev_ord[MAX_LEVELS];
  int size; // următoarea celulă nealocată

  // Folosește celulele 0 și 1 pentru santinele: turnuri de MAX_LEVELS
  // pointeri pentru -∞ și +∞. Toți pointerii de la -∞ pointează la +∞, la
  // distanță 1.
  void init() {
    size = 2;
    a[0].val = -INF;
    a[1].val = +INF;
    a[0].height = a[1].height = MAX_LEVELS;
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
      a[0].dist[l] = 1;
    }
  }

  // Alege o înălțime de turn din distribuția geometrică cu p = 0,5.
  // Rezultatul returnat este în [1, MAX_LEVELS).
  int get_height() {
    int r = rand() | (1 << (MAX_LEVELS - 2));
    return 1 + __builtin_ctz(r);
  }

  void insert(int val) {
    // Atribuie valoarea și creează turnul de pointeri și de distanțe.
    a[size].val = val;
    int h = get_height();
    a[size].height = h;

    int pos = 0, order = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      // Avansează pe orizontală pe nivelul l, doar peste valori strict mai
      // mici. Calculează și indicele inserării.
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }

      // Interpune celula size între pos și următoarea celulă.
      a[pos].dist[l]++; // Include și nodul însuși.
      if (l < h) {
        a[size].next[l] = a[pos].next[l];
        a[pos].next[l] = size;
        prev[l] = pos;
        prev_ord[l] = order;
      }
    }

    // Acum că ne știm propriul indice, actualizează distanțele.
    order++;
    for (int l = 0; l < h; l++) {
      a[size].dist[l] = a[prev[l]].dist[l] - (order - prev_ord[l]);
      a[prev[l]].dist[l] = order - prev_ord[l];
    }

    size++;
  }

  void erase(int val) {
    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }

      a[pos].dist[l]--;
      if (a[a[pos].next[l]].val == val) {
        a[pos].dist[l] += a[a[pos].next[l]].dist[l];
        a[pos].next[l] = a[a[pos].next[l]].next[l];
      }
    }

    size--;
  }

  // Avansează pe fiecare nivel, doar peste valori strict mai mici.
  bool contains(int val) {
    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
    }

    // Următoarea celulă ar putea fi cea căutată.
    return (a[a[pos].next[0]].val == val);
  }

  // Presupune că valoarea există și este unică.
  int order_of(int val) {
    int pos = 0, order = 0;

    // Cod identic cu cel de la inserare.
    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        order += a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    return order;
  }

  int kth_element(int k) {
    int pos = 0;
    k++; // Sari peste santinela -∞.

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      // Suma distanțelor nu trebuie să depășească k.
      while (a[pos].dist[l] <= k) {
        k -= a[pos].dist[l];
        pos = a[pos].next[l];
      }
    }

    return a[pos].val;
  }
};

skip_list sl;
int perm[N];

void gen_perm() {
  // Amestecă mulțimea {0, 1, ..., N - 1}.
  for (int i = 0; i < N; i++) {
    int j = rand() % (i + 1);
    perm[i] = perm[j];
    perm[j] = i;
  }
}

int main() {
  srand(time(NULL));
  gen_perm();

  // Exemplu de folosire și test de corectitudine.
  sl.init();

  for (int i = 0; i < N; i++) {
    sl.insert(perm[i]);
    int j = rand() % N;
    assert(sl.contains(perm[j]) == (j <= i));
  }

  for (int i = 0; i < N; i++) {
    assert(sl.order_of(i) == i);
    assert(sl.kth_element(i) == i);
  }

  for (int i = 0; i < N; i++) {
    sl.erase(perm[i]);
    int j = rand() % N;
    assert(sl.contains(perm[j]) == (j > i));
  }

  return 0;
}
