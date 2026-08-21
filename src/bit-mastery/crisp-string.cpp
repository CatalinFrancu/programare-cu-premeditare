#include <stdio.h>

const int MAX_LEN = 100'000;
const int MAX_SIGMA = 17;
const int NONE = -1;

struct bitset {
  int n, num_words;
  unsigned v[(1 << MAX_SIGMA) / 32];

  void init(int n) {
    this->n = n;
    num_words = (n - 1) / 32 + 1;
  }

  void clear() {
    for (int i = 0; i < num_words; i++) {
      v[i] = 0;
    }
  }

  bool get(int pos) {
    return (v[pos >> 5] >> (pos & 31)) & 1;
  }

  void set(int pos) {
    v[pos >> 5] |= 1 << (pos & 31);
  }

  bitset& operator|=(const bitset& other) {
    for (int i = 0; i < num_words; i++) {
      v[i] |= other.v[i];
    }
    return *this;
  }
};

char s[MAX_LEN + 1];
bool compat[MAX_SIGMA][MAX_SIGMA];
int freq[MAX_SIGMA];
bitset bad, global_bad;
int n, sigma;

void read_data() {
  scanf("%d %d %s", &n, &sigma, s);
  for (int i = 0; i < sigma; i++) {
    for (int j = 0; j < sigma; j++) {
      int x;
      scanf("%d", &x);
      compat[i][j] = x;
    }
  }
}

void translate_string() {
  for (int i = 0; i < n; i++) {
    s[i] -= 'a';
    freq[(int)s[i]]++;
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void include_submasks(int x, int y) {
  // Iterează doar prin măștile care au biții x și y nesetați.
  unsigned mask = ((1 << sigma) - 1) ^ ((1 << x) | (1 << y));

  bad.set(mask);
  for (unsigned s = mask; s; s = (s - 1) & mask) {
    // s iterează prin submăști în ordine descrescătoare. Noi dorim să iterăm
    // în ordine crescătoare.
    unsigned t = mask ^ s;
    if (!bad.get(t)) {
      for (int bit = 0; bit < sigma; bit++) {
        if ((t & (1 << bit)) && bad.get(t ^ (1 << bit))) {
          bad.set(t);
        }
      }
    }
  }
}

void gen_bad_masks_for_pair(int x, int y) {
  int last_seen = NONE;
  unsigned mask = 0;
  for (int i = 0; i < n; i++) {
    if ((s[i] == x) || (s[i] == y)) {
      if (last_seen == x + y - s[i]) { // cealaltă valoare
        bad.set(mask);
      }
      mask = 0;
      last_seen = s[i];
    } else {
      mask |= (1 << s[i]);
    }
  }

  include_submasks(x, y);
  global_bad |= bad;
  bad.clear();
}

void gen_bad_masks() {
  bad.init(1 << sigma);
  global_bad.init(1 << sigma);

  for (int x = 0; x < sigma; x++) {
    for (int y = x; y < sigma; y++) {
      if (freq[x] && freq[y] && !compat[x][y]) {
        gen_bad_masks_for_pair(x, y);
      }
    }
  }
}

// Marchează care rele toate măștile inaccesibile din 0 prin orice secvență de
// eliminări.
void gen_unreachable_masks() {
  for (unsigned mask = 1; mask < (1u << sigma); mask++) {
    if (!global_bad.get(mask)) {
      bool reachable = false;
      for (int bit = 0; bit < sigma; bit++) {
        reachable |=
          (mask & (1 << bit)) &&
          !global_bad.get(mask ^ (1 << bit));
      }
      if (!reachable) {
        global_bad.set(mask);
      }
    }
  }
}

int get_min_pop() {
  int result = n;
  for (unsigned mask = 0; mask < (1u << sigma); mask++) {
    if (!global_bad.get(mask)) {
      int pop = 0;
      for (int bit = 0; bit < sigma; bit++) {
        if (!(mask & (1 << bit))) {
          pop += freq[bit];
        }
      }
      result = min(result, pop);
    }
  }
  return result;
}

int main() {
  read_data();
  translate_string();
  gen_bad_masks();
  gen_unreachable_masks();
  int answer = get_min_pop();
  printf("%d\n", answer);

  return 0;
}
