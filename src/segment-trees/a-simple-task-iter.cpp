#include <stdio.h>

const int MAX_LEN = 1 << 17;
const int SIGMA = 26;

const int ST_DESC = 0;
const int ST_ASC = 1;
const int ST_CLEAN = 2;

// Parametrii necesari pentru a itera crescător / descrescător.
const int LOOP_START[2] = { SIGMA - 1, 0 };
const int LOOP_END[2]   = {        -1, SIGMA };
const int LOOP_STEP[2]  = {        -1, +1 };

int min(int x, int y) {
  return (x < y) ? x : y;
}

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

struct node {
  int f[SIGMA];
  char state;

  int get_first_nonzero() {
    int i = 0;
    while (!f[i]) {
      i++;
    }
    return i;
  }

  void add(node& a, node& b) {
    for (int i = 0; i < SIGMA; i++) {
      f[i] = a.f[i] + b.f[i];
    }
  }

  void accumulate(node& src) {
    for (int i = 0; i < SIGMA; i++) {
      f[i] += src.f[i];
    }
  }

  // Sparge this în a și b. Îl lasă intact pe this.
  void split(node& a, node& b, int a_size, int dir) {
    for (int i = LOOP_START[dir]; i != LOOP_END[dir]; i += LOOP_STEP[dir]) {
      int to_a = min(a_size, f[i]);
      a.f[i] = to_a;
      b.f[i] = f[i] - to_a;
      a_size -= to_a;
    }
    a.state = b.state = dir;
    state = ST_CLEAN;
  }

  // Distribuie quantity litere din this în dest.
  void distribute(node& dest, int quantity, int dir, int new_state) {
    for (int i = LOOP_START[dir]; i != LOOP_END[dir]; i += LOOP_STEP[dir]) {
      int to_dest = min(quantity, f[i]);
      dest.f[i] = to_dest;
      f[i] -= to_dest;
      quantity -= to_dest;
    }
    dest.state = new_state;
  }
};

struct segment_tree {
  node v[2 * MAX_LEN], acc;
  int n, bits;

  void init(char* s, int len) {
    n = next_power_of_2(len);
    bits = 31 - __builtin_clz(n);

    for (int i = 0; i < len; i++) {
      int c = s[i] - 'a';
      for (int t = n + i; t; t /= 2) {
        v[t].f[c]++;
      }
    }

    for (int i = 1; i < 2 * n; i++) {
      v[i].state = ST_CLEAN;
    }
  }

  void push(int pos, int size) {
    if (v[pos].state != ST_CLEAN) {
      int l = 2 * pos, r = 2 * pos + 1;
      v[pos].split(v[l], v[r], size / 2, v[pos].state);
    }
  }

  void push_path(int node) {
    for (int b = bits, size = n; b; b--, size >>= 1) {
      push(node >> b, size);
    }
  }

  void push_all(int node, int size) {
    if (node < n) {
      push(node, size);
      push_all(2 * node, size / 2);
      push_all(2 * node + 1, size / 2);
    }
  }

  void pull_path(int pos) {
    for (pos /= 2; pos; pos /= 2) {
      // Nu trage frecvențele literelor din fii în noduri dirty. Pot fi
      // învechite.
      if (v[pos].state == ST_CLEAN) {
        v[pos].add(v[2 * pos], v[2 * pos + 1]);
      }
    }
  }

  void collect(int l, int r) {
    l += n;
    r += n;
    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        acc.accumulate(v[l++]);
      }
      l >>= 1;

      if (!(r & 1)) {
        acc.accumulate(v[r--]);
      }
      r >>= 1;
    }
  }

  void distribute(int l, int r, int dir) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;
    int size = 1;

    while (l <= r)  {
      if (l & 1) {
        acc.distribute(v[l++], size, dir, dir);
      }
      l >>= 1;

      if (!(r & 1)) {
        acc.distribute(v[r--], size, 1 - dir, dir);
      }
      r >>= 1;
      size <<= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  void sort(int l, int r, int dir) {
    collect(l, r);
    distribute(l, r, dir);
  }

  void export_(char* dest, int len) {
    push_all(1, n);

    for (int i = 0; i < len; i++) {
      dest[i] = 'a' + v[n + i].get_first_nonzero();
    }
  }
};

char s[MAX_LEN + 2];
segment_tree st;
int len, num_ops;

void read_string() {
  scanf("%d %d %s", &len, &num_ops, s);
}

void process_ops() {
  int left, right, dir;

  while (num_ops--) {
    scanf("%d %d %d", &left, &right, &dir);
    st.sort(left - 1, right - 1, dir);
  }
}

void write_answer() {
  printf("%s\n", s);
}

int main() {
  read_string();
  st.init(s, len);
  process_ops();
  st.export_(s, len);
  write_answer();

  return 0;
}
