#include <stdio.h>

const int MAX_COLORS = 40;
const int INFINITY = 1'000'000'000;

typedef unsigned long long u64;

u64 rel[MAX_COLORS];
int cost[MAX_COLORS];
u64 rhs_mask;
int num_colors, mid;

void add_relation(int a, int b) {
  rel[a] |= (1ull << b);
  rel[b] |= (1ull << a);
}

// Adaugă relațiile <c[1],c[1]> și <c[n],c[n]> pentru a forța alegerea acestor
// două culori.
void read_data() {
  int n, prev, col;
  scanf("%d %d %d", &n, &num_colors, &prev);
  prev--;

  add_relation(prev, prev);
  for (int i = 1; i < n; i++) {
    scanf("%d", &col);
    col--;
    add_relation(prev, col);
    prev = col;
  }
  add_relation(prev, prev);

  for (int i = 0; i < num_colors; i++) {
    scanf("%d", &cost[i]);
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

int log2(u64 mask) {
  return 63 - __builtin_clzll(mask);
}

// Returnează o mască cu 1 de la msb la lsb inclusiv.
u64 bit_range(int msb, int lsb) {
  return ((1ull << (msb + 1)) - 1) ^ ((1ull << lsb) - 1);
}

void precompute_info() {
  mid = num_colors / 2;
  rhs_mask = bit_range(mid - 1, 0);
}

struct right_half {
  // Costul redus al unei măști este costul minim necesar pentru a satisface
  // relațiile între perechi de biți 1 din mască.
  int red_cost[1 << (MAX_COLORS / 2)];

  int get_mask_cost(u64 mask) {
    int sum = 0;
    while (mask) {
      int b = log2(mask);
      sum += cost[b];
      mask ^= (1 << b);
    }
    return sum;
  }

  void compute_red_costs() {
    for (u64 mask = 1; mask < (1ull << mid); mask++) {
      int k = log2(mask);

      // Opțiunea 1: cumpără bitul k. Redu biții rămași.
      u64 remaining = mask ^ (1 << k);
      int c1 = cost[k] + red_cost[remaining];

      // Opțiunea 2: nu cumpăra bitul k. Neapărat cumpără-i toate relațiile.
      // Aceasta tratează și cazul în care k este în relație cu el însuși.
      u64 rel_mask = mask & rel[k];
      remaining = (mask ^ (1 << k)) & ~rel[k];
      int c2 = get_mask_cost(rel_mask) + red_cost[remaining];

      red_cost[mask] = min(c1, c2);
    }
  }

  int cheapest_expansion(u64 bought_mask) {
    u64 not_bought = rhs_mask ^ bought_mask;
    return get_mask_cost(bought_mask) + red_cost[not_bought];
  }
};

right_half right;

struct left_half {
  int min_cost;

  void rec(int k, int c, u64 left_mask, u64 right_mask) {
    if (k == num_colors) {
      int right_cost = right.cheapest_expansion(right_mask);
      min_cost = min(min_cost, c + right_cost);
    } else {
      // Trebuie să cumpărăm bitul k dacă are relații nesatisfăcute în
      // jumătatea stîngă unde k este bitul mai semnificativ.
      u64 unsat = rel[k] & bit_range(k, mid) & ~left_mask;
      if (!unsat) {
        // Putem să nu cumpărăm bitul k. Notează relațiile lui k în jumătatea
        // dreaptă.
        u64 rhs_relations = rel[k] & rhs_mask;
        rec(k + 1, c, left_mask, right_mask | rhs_relations);
      }
      rec(k + 1, c + cost[k], left_mask | (1ull << k), right_mask);
    }
  }

  void generate_all_valid() {
    min_cost = INFINITY;
    rec(mid, 0, 0, 0);
  }
};

left_half left;

int main() {
  read_data();
  precompute_info();
  right.compute_red_costs();
  left.generate_all_valid();
  printf("%d\n", left.min_cost);

  return 0;
}
