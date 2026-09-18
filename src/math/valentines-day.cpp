// Complexitate: O(n log n log* n).
#include <set>
#include <stdio.h>

const int MAX_N = 300'000;

int a[MAX_N], p[MAX_N];
int freq[MAX_N + 1];
int choices[MAX_N + 1];
int n, k;

// Menține o mulțime de numere și produsul lor, limitat la k.
struct factor_set {
  int cnt[MAX_N + 1]; // cnt[x] = numărul de factori egali cu x
  int pos[MAX_N + 1]; // poziția lui x în lst[]
  int lst[MAX_N + 1]; // listă de factori distincți
  int n;              // numărul de factori distincți (mărimea lui lst)
  int total;          // numărul de factori

  void init() {
    n = total = 0;
  }

  void insert(int f) {
    if (f > 1) {
      cnt[f]++;
      total++;
      if (cnt[f] == 1) {
        pos[f] = n;
        lst[n++] = f;
      }
    }
  }

  void erase(int f) {
    if (f > 1) {
      cnt[f]--;
      total--;
      if (!cnt[f]) {
        n--;
        pos[lst[n]] = pos[f];
        lst[pos[f]] = lst[n];
      }
    }
  }

  int slow_product() {
    long long p = 1;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < cnt[lst[i]]; j++) {
        p *= lst[i];
        if (p >= k) {
          return k;
        }
      }
    }
    return p;
  }

  int product() {
    if (total >= 31) {
      // Nu strică și poate ajuta ocazional.
      return k;
    } else {
      return slow_product();
    }
  }
};

factor_set fs;

struct disjoint_set_forest {
  int p[MAX_N + 1];

  void init(int n) {
    for (int u = 1; u <= n; u++) {
      p[u] = u;
    }
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  // Unifică stînga cu dreapta. Se așteaptă ca v > u. Fără union by rank.
  void unite(int u, int v) {
    p[find(u)] = find(v);
  }
};

disjoint_set_forest hand; // valorile din mînă, de așezat în p

void read_data() {
  scanf("%d %d", &n, &k);
  for (int i = 0; i < n; i++) {
    scanf("%d", &a[i]);
    freq[a[i]]++;
  }
}

void init_choices() {
  fs.init();

  int constr = 0; // numărul de constrîngeri <= val
  for (int val = 1; val <= n; val++) {
    constr += freq[val];
    choices[val] = constr - (val - 1); // val - 1 elemente au ales deja
    fs.insert(choices[val]);
  }
}

int find_value_to_satisfy(int constr) {
  int val = hand.find(constr);

  fs.erase(choices[val]);

  // Încearcă să avansezi din *it cît timp nu lăsăm în urmă mai mult de k
  // permutări.
  int p;
  while ((p = fs.product()) < k) {
    // Luăm constrîngerea constr, care este ≤ *it, așadar *it pierde o
    // variantă chiar dacă mai tîrziu alegem o valoare > *it.
    k -= p;
    choices[val]--;
    fs.insert(choices[val]);
    val = hand.find(val + 1);
    fs.erase(choices[val]);
  }

  return val;
}

void fill_permutation() {
  hand.init(n);

  for (int i = 0; i < n; i++) {
    p[i] = find_value_to_satisfy(a[i]);
    hand.unite(p[i], p[i] + 1);
  }
}

void write_answer() {
  for (int i = 0; i < n; i++) {
    printf("%d ", p[i]);
  }
  printf("\n");
}

int main() {
  read_data();
  init_choices();
  fill_permutation();
  write_answer();

  return 0;
}
