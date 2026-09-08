// Cînd ai un ciocan, totul în jurul tău pare un cui. Fără legătură cu asta,
// iată o soluție cu șiruri de sufixe + LCP + RMQ + chiuveta din bucătărie.
#include <algorithm>
#include <stdio.h>
#include <string.h>

const int MAX_N = 100'000;
const int MAX_LOG = 17;

struct suffix_array {
  const char* s;
  int n, num_classes;

  int p[MAX_N + 1];
  int c[MAX_N + 1];
  int p2[MAX_N + 1];
  int c2[MAX_N + 1];

  int cnt[256];

  int lcp[MAX_N + 1];

  void init(char* s, int n) {
    this->s = s;
    this->n = n;
  }

  int circ(int x, int n) {
    return (x >= n) ? (x - n) : x;
  }

  void build_single_letters() {
    memset(cnt, 0, sizeof(int) * 256);
    for (int i = 0; i < n; i++) {
      cnt[(int)s[i]]++;
    }
    for (int i = 1; i <= 'z'; i++) {
      cnt[i] += cnt[i - 1];
    }

    for (int i = 0; i < n; i++) {
      p[--cnt[(int)s[i]]] = i;
    }

    c[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      c[p[i]] = c[p[i - 1]] + (s[p[i]] != s[p[i - 1]]);
    }
    num_classes = 1 + c[p[n - 1]];
  }

  void build_prefix_len(int len) {
    memset(cnt, 0, sizeof(int) * num_classes);
    for (int i = 0; i < n; i++) {
      cnt[c[i]]++;
    }
    for (int i = 1; i < num_classes; i++) {
      cnt[i] += cnt[i - 1];
    }

    for (int i = 0; i < n; i++) {
      p2[i] = circ(p[i] - len + n, n);
    }
    for (int i = n - 1; i >= 0; i--) {
      p[--cnt[c[p2[i]]]] = p2[i];
    }

    c2[p[0]] = 0;
    for (int i = 1; i < n; i++) {
      bool diff =  (c[p[i]] != c[p[i - 1]]) ||
        (c[circ(p[i] + len, n)] != c[circ(p[i - 1] + len, n)]);
      c2[p[i]] = c2[p[i - 1]] + diff;
    }
    num_classes = 1 + c2[p[n - 1]];
    memcpy(c, c2, sizeof(int) * n);
  }

  void build() {
    build_single_letters();

    int len = 1;
    while (num_classes < n) {
      build_prefix_len(len);
      len <<= 1;
    }

    // Permutarea inversă
    for (int i = 0; i < n; i++) {
      p2[p[i]] = i;
    }
  }

  void build_lcp() {
    int l = 0;
    for (int i = 0; i < n; i++) {
      int k = c[i];
      if (k) {
        int j = p[k - 1];
        while (s[i + l] == s[j + l]) {
          l++;
        }
        lcp[k] = l;

        if (l) {
          l--;
        }
      }
    }
  }
};

int log2(int x) {
  return 31 - __builtin_clz(x);
}

struct sparse_table {
  int v[MAX_LOG][MAX_N + 1];

  void build(int* src, int n) {
    for (int i = 0; i < n; i++) {
      v[0][i] = src[i];
    }
    for (int p = 1; (1 << p) <= n; p++) {
      for (int i = 0; i <= n - (1 << p); i++) {
        v[p][i] = std::min(v[p - 1][i], v[p - 1][i + (1 << (p - 1))]);
      }
    }
  }

  int range_min(int l, int r) {
    int row = log2(r - l + 1);
    return std::min(v[row][l], v[row][r - (1 << row) + 1]);
  }
};

char s[MAX_N + 1];
int spf[MAX_N + 1]; // spf[x] = cel mai mic factor prim al lui x
suffix_array suf;
sparse_table st;
int n;

void read_string() {
  scanf("%d %s", &n, s);
}

void sieve() {
  spf[1] = 1;
  for (int i = 2; i <= n; i++) {
    if (!spf[i])  {
      for (int j = i; j <= n; j += i) {
        if (!spf[j]) {
          spf[j] = i;
        }
      }
    }
  }
}

void the_mother_of_all_preprocessing() {
  sieve();
  suf.init(s, n + 1);
  suf.build();
  suf.build_lcp();
  st.build(suf.lcp, n + 1);
}

// Este necesar ca sufixele de la l și l + per să aibă un prefix comun de cel
// puțin (r - l + 1) - per caractere.
bool matches(int l, int r, int per) {
  int x = suf.p2[l], y = suf.p2[l + per];
  if (x > y) {
    std::swap(x, y);
  }

  int lcp = st.range_min(x + 1, y);
  return lcp >= (r - l + 1) - per;
}

int query(int l, int r) {
  int len = r - l + 1;
  bool found = false;

  while (!found && (len > 1)) {
    int x = spf[len];
    found = matches(l, r, (r - l + 1) / x);
    do {
      len /= x;
    } while (len % x == 0);
  }

  return found;
}

void process_queries() {
  int q, l, r;

  scanf("%d", &q);
  while (q--) {
    scanf("%d %d", &l, &r);
    printf(query(l, r) ? "YES\n" : "NO\n");
  }
}

int main() {
  read_string();
  the_mother_of_all_preprocessing();
  process_queries();

  return 0;
}
