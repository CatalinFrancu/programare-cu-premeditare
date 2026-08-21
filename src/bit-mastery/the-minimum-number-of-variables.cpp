#include <stdio.h>

const int MAX_N = 23;
const int NONE = -1;

struct bitset {
  unsigned v[(1 << MAX_N) / 32];

  void set(int pos) {
    v[pos >> 5] |= 1 << (pos & 31);
  }

  bool get(int pos) {
    return v[pos >> 5] >> (pos & 31) & 1;
  }
};

// poss[mask] = true dacă și numai dacă putem procesa primele i variabile
// a.î. variabilele curente să fie cele dictate de mască. Notă: i este dat de
// bitul cel mai semnificativ al măștii.
bitset poss;

// sum[mask] = true dacă și numai dacă variabilele dictate de mască pot fi
// folosite ca să obținem a i+1-a variabilă.
bitset sum;

int a[MAX_N], n;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int log2(int x) {
  return 31 - __builtin_clz(x);
}

// Presupune că submăștile au fost deja actualizate.
bool can_obtain_sum(unsigned mask, int target) {
  int lsb = log2(mask & -mask);
  if (sum.get(mask ^ (1 << lsb))) {
    return true;
  }

  while (mask) {
    int bit = log2(mask);
    mask ^= (1 << bit);
    if (a[lsb] + a[bit] == target) {
      return true;
    }
  }

  return false;
}

// Menține măștile pe bit-1 biți care pot genera a[bit].
void update_sum(int bit) {
  for (unsigned mask = (1u << (bit - 1)); mask < (1u << bit); mask++) {
    if (can_obtain_sum(mask, a[bit])) {
      sum.set(mask);
    }
  }
}

void set_all_continuations(unsigned mask, int bit) {
  poss.set(mask ^ (1 << bit)); // folosește o variabilă nouă
  unsigned cp = mask;
  while (cp) {
    unsigned lsb = cp & -cp;
    poss.set(mask ^ (1 << bit) ^ lsb); // înlocuiește o variabilă existentă
    cp ^= lsb;
  }
}

void update_poss(int bit) {
  for (unsigned mask = (1u << (bit - 1)); mask < (1u << bit); mask++) {
    if (poss.get(mask) && sum.get(mask)) {
      set_all_continuations(mask, bit);
    }
  }
}

void visit_all_bits() {
  // putem procesa obiectul 0 (mask 000...001 = 1)
  poss.set(1);

  for (int bit = 1; bit < n; bit++) {
    update_sum(bit);
    update_poss(bit);
  }
}

// Returnează populația minimă printre măștile care au bitul n-1 setat.
int get_min_pop_mask() {
  int min_pop = n + 1;
  for (unsigned mask = 1 << (n - 1); mask < (1u << n); mask++) {
    if (poss.get(mask)) {
      min_pop = min(min_pop, __builtin_popcount(mask));
    }
  }
  return (min_pop == n + 1) ? NONE : min_pop;
}

int main() {
  read_data();
  visit_all_bits();
  int answer = get_min_pop_mask();
  printf("%d\n", answer);

  return 0;
}
