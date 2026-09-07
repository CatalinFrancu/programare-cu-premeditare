#include <stdio.h>
#include <string.h>

const int MAX_N = 300;
const long long BASE = 1e16;
const int DIGIT_GROUP = 16;
const int MAX_LEN = 40; // 300! < 10^615 < 10^{16*40}

int min(int x, int y) {
  return (x < y) ? x : y;
}

struct big_number {
  int len;
  long long v[MAX_LEN + 1];

  void init() {
    len = 1;
    v[0] = 1;
  }

  void read(FILE* f) {
    char s[MAX_LEN * DIGIT_GROUP];
    fscanf(f, "%s", s);
    int slen = strlen(s);
    long long coef = 1;
    for (int i = 0; i < slen; i++) {
      int digit = s[slen - 1 - i] - '0';
      if (i % DIGIT_GROUP == 0) {
        coef = 1;
        len++;
      }
      v[len - 1] += digit * coef;
      coef *= 10;
    }
  }

  void mult(int x) {
    int i = 0;
    long long carry = 0;
    while ((i < len) || carry) {
      carry = v[i] * x + carry;
      v[i] = carry % BASE;
      carry /= BASE;
      i++;
    }
    len = i;
  }

  void shrink() {
    while (len && !v[len - 1]) {
      len--;
    }
  }

  void div(int x) {
    long long carry = 0;
    for (int i = len - 1; i >= 0; i--) {
      carry = carry * BASE + v[i];
      v[i] = carry / x;
      carry %= x;
    }

    shrink();
  }

  // this = a - b. Funcționează și dacă this == a (deci calculează a -= b).
  void diff(big_number& a, big_number& b) {
    long long borrow = 0;
    len = a.len;

    for (int i = b.len; i < a.len; i++) {
      b.v[i] = 0;
    }

    for (int i = 0; i < len; i++) {
      v[i] = a.v[i] - b.v[i] - borrow;
      borrow = (v[i] < 0);
      v[i] += BASE * borrow;
    }

    shrink();
  }

  bool less_than(big_number& other) {
    if (len != other.len) {
      return (len < other.len);
    }

    int i = len - 1;
    while ((i >= 0) && (v[i] == other.v[i])) {
      i--;
    }
    return ((i >= 0) && (v[i] < other.v[i]));
  }
};

struct perm_counter {
  int choices[MAX_N];
  big_number count;
  int saved_choice;
  int n, m;

  void init(int n, int m) {
    this->n = n;
    this->m = m;
    count.init();

    for (int i = 0; i < n; i++) {
      choices[i] = min(m, n - i);
      count.mult(choices[i]);
    }
  }

  void clear(int i) {
    saved_choice = choices[i];
    if (choices[i]) {
      count.div(choices[i]);
    }
    choices[i] = 1;
  }

  void restore(int i) {
    choices[i] = saved_choice - 1;
    count.mult(choices[i]);
  }
};

perm_counter a, b;
big_number k; // 1-based ca să nu mai implementez decrementarea pe big_number
bool used[MAX_N + 1];
int sol[MAX_N];
int n, m;

void read_data() {
  FILE* f = fopen("bubblesort.in", "r");
  fscanf(f, "%d %d ", &n, &m);
  k.read(f);
  fclose(f);
}

void unrank() {
  a.init(n, m);
  b.init(n, m - 1);

  for (int i = 0; i < n; i++) {
    int val = -1;
    bool found = false;
    do {
      val++;
      if (!used[val]) {
        a.clear(val);
        b.clear(val);
        big_number count;
        count.diff(a.count, b.count);
        if (count.less_than(k)) {
          k.diff(k, count);
          a.restore(val);
          b.restore(val);
        } else {
          found = true;
        }
      }
    } while (!found);

    sol[i] = val;
    used[val] = true;
  }
}

void write_perm() {
  FILE* f = fopen("bubblesort.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(f, "%d%c", sol[i] + 1, (i < n - 1) ? ' ' : '\n');
  }
  fclose(f);
}

int main() {
  read_data();
  unrank();
  write_perm();

  return 0;
}
