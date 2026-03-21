#include <assert.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less<int>,
  __gnu_pbds::rb_tree_tag,
  __gnu_pbds::tree_order_statistics_node_update
  > ordered_set;

const int MAX_LEVELS = 20;
const int MAX_NODES = 300'000;
const int INF = 2'000'000'000;

struct node {
  int val;
  int height;
  int* next;
  int* dist;
};

struct skip_list {
  node a[MAX_NODES + 2];
  int buf[MAX_NODES * 5], buf_ptr; // buffer pentru pointeri și distanțe
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
    a[0].next = buf;
    a[0].dist = buf + MAX_LEVELS;
    buf_ptr = 2 * MAX_LEVELS;
    for (int l = 0; l < MAX_LEVELS; l++) {
      a[0].next[l] = 1;
      a[0].dist[l] = 1;
    }
  }

  // Alege o înălțime de turn din distribuția geometrică cu p = 0,5.
  int get_height() {
    return 1 + __builtin_ctz(rand());
  }

  void insert(int val) {
    // Atribuie valoarea și creează turnul de pointeri și de distanțe.
    a[size].val = val;
    int h = get_height();
    a[size].height = h;
    a[size].next = buf + buf_ptr;
    a[size].dist = buf + buf_ptr + h;
    buf_ptr += 2 * h;

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

    // Ar trebui să returnăm elementul următor, dar pe de altă parte am pornit
    // de pe santinela -∞.
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
ordered_set stl_set;

int main() {
  srand(time(NULL));

  sl.init();

  // Exemplu de folosire și test de corectitudine. Inserează aceleași valori
  // în skip list și într-un set PBDS și verifică că ordinile coincid.
  for (int i = 0; i < MAX_NODES; i++) {
    int x = rand() % INF;
    // For simplicity, don't deal with multisets.
    if (!sl.contains(x)) {
      sl.insert(x);
      stl_set.insert(x);
      assert(sl.order_of(x) == (int)stl_set.order_of_key(x));
    }
  }

  // Verifică că al k-lea element coincide.
  for (int i = stl_set.size() - 1; i >= 0; i--) {
    assert(sl.kth_element(i) == *stl_set.find_by_order(i));
  }

  return 0;
}
