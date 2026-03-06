// Metodă calculăm rangul lui P și Q în baza factorial. Adunăm rangurile și
// ignorăm ultimul transport (peste cifra a N-a). Calculăm permutarea pentru
// rangul-sumă.
#include <stdio.h>

const int MAX_N  = 200'000;

int r[MAX_N], n; // r[] acumulează rangurile lui P și Q

struct fenwick_tree {
  int v[MAX_N + 1];
  int n, max_p2;

  void init(int n) {
    this->n = n;
    max_p2 = 1 << (31 - __builtin_clz(n));
    for (int i = 1; i <= n; i++) {
      v[i] = 0;
    }
  }

  void add(int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  int prefix_sum(int pos) {
    int s = 0;
    while (pos) {
      s += v[pos];
      pos &= pos - 1;
    }
    return s;
  }

  int bin_search(int val) {
    int pos = 0;

    for (int interval = max_p2; interval; interval >>= 1) {
      if ((pos + interval <= n) && (v[pos + interval] < val)) {
        val -= v[pos + interval];
        pos += interval;
      }
    }

    return pos;
  }
};

fenwick_tree fen;

// Citește o permutare și îi adaugă rangul factorial în r[].
void read_and_rank() {
  fen.init(n);

  for (int i = n - 1; i >= 0; i--) {
    int x;
    scanf("%d", &x);
    x++;
    int cnt = fen.prefix_sum(x);
    r[i] += (x - 1) - cnt;
    fen.add(x, 1);
  }
}

void carry() {
  int t = 0;
  for (int i = 0; i < n; i++) {
    r[i] += t;
    t = (r[i] > i);
    r[i] -= t * (i + 1);
  }
}

// Face unrank la r[] și tipărește permutarea.
void unrank_and_write() {
  // Valorile de 1 din AIB indică elemente folosibile.
  // Avem deja toate valorile pe 1 ca urmare a lui read_and_rank().
  for (int i = n - 1; i >= 0; i--) {
    // Găsește cea de-a r[i]-a valoare nefolosită.
    int x = fen.bin_search(r[i] + 1);
    printf("%d ", x);
    fen.add(x + 1, -1);
  }
  printf("\n");
}

int main() {
  scanf("%d", &n);
  read_and_rank();
  read_and_rank();
  carry();
  unrank_and_write();

  return 0;
}
