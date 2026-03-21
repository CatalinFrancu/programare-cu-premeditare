#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>

const int MAX_LEVELS = 20;
const int MAX_N = 300'000;
const int INF = 2'000'000'000;

struct node {
  int val;
  int height;
  int* next;
};

struct skip_list {
  node a[MAX_N + 2];
  int buf[MAX_N * 5 / 2], buf_ptr; // buffer global pentru turnuri de pointeri
  int size; // următoarea celulă nealocată

  // Folosește celulele 0 și 1 pentru santinele: turnuri de MAX_LEVELS
  // pointeri pentru -∞ și +∞. Toți pointerii de la -∞ pointează la +∞.
  void init() {
    size = 2;
    a[0].val = -INF;
    a[1].val = +INF;
    a[0].height = a[1].height = MAX_LEVELS;
    a[0].next = buf;
    buf_ptr = MAX_LEVELS;
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
    }
  }

  // Alege o înălțime de turn din distribuția geometrică cu p = 0,5.
  int get_height() {
    return 1 + __builtin_ctz(rand());
  }

  void insert(int val) {
    // Atribuie valoarea și creează turnul de pointeri.
    a[size].val = val;
    a[size].height = get_height();
    a[size].next = buf + buf_ptr;
    buf_ptr += a[size].height;

    int pos = 0;

    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      // Avansează pe orizontală pe nivelul l, doar peste valori strict mai
      // mici.
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
      // Interpune celula size între pos și următoarea celulă.
      if (l < a[size].height) {
        a[size].next[l] = a[pos].next[l];
        a[pos].next[l] = size;
      }
    }

    size++;
  }

  bool contains(int val) {
    int pos = 0;

    // Avansează pe fiecare nivel, doar peste valori strict mai mici.
    for (int l = MAX_LEVELS - 1; l >= 0; l--) {
      while (a[a[pos].next[l]].val < val) {
        pos = a[pos].next[l];
      }
    }

    // Următoarea celulă ar putea fi cea căutată.
    return (a[a[pos].next[0]].val == val);
  }
};

skip_list sl;
std::unordered_set<int> stl_set;
int values[MAX_N];

int main() {
  srand(time(NULL));

  for (int i = 0; i < MAX_N; i++) {
    values[i] = rand() % INF;
  }

  // Exemplu de folosire și test de corectitudine. Inserează și caută aceleași
  // valori în skip list și într-un unsorted set și verifică că coincid.
  sl.init();

  for (int i = 0; i < MAX_N; i++) {
    sl.insert(values[i]);
    stl_set.insert(values[i]);
    int j = rand() % MAX_N;
    assert(sl.contains(values[j]) == stl_set.contains(values[j]));
  }

  return 0;
}
