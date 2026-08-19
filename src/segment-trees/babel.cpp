#include <stdio.h>

const int MAX_N = 200'000;
const int MAX_POW_2 = 1 << 18;
const int MOD = 666'013;
const int NONE = 3;

const int mod3[6] = { 0, 1, 2, 0, 1, 2 };
int pow4[MAX_N / 2];

int next_power_of_2(int x) {
  return 1 << (32 - __builtin_clz(x - 1));
}

// Un aint cu operațiile de atribuire pe interval și evaluare punctuală.
// Nodurile în care se descompune o atribuire rețin timpul și valoarea
// atribuirii.
struct r_segment_tree_node {
  int time;
  int val;
};

struct r_segment_tree {
  r_segment_tree_node v[2 * MAX_POW_2];
  int n, time;

  void init(int _n) {
    n = next_power_of_2(_n);
    time = 0;
  }

  void set(int pos, int val) {
    v[pos + n] = { 0, val };
  }

  void range_set(int l, int r, int val) {
    l += n;
    r += n;
    time++;

    while (l <= r)  {
      if (l & 1) {
        v[l++] = { time, val };
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r--] = { time, val };
      }
      r >>= 1;
    }
  }

  // Returnează valoarea cu timestamp-ul cel mai mare dintre toți strămoșii.
  int get(int pos) {
    pos += n;
    int result = v[pos].val;
    int max_time = v[pos].time;
    for (pos /= 2; pos; pos /= 2) {
      if (v[pos].time > max_time) {
        result = v[pos].val;
        max_time = v[pos].time;
      }
    }
    return result;
  }
};

struct segment_tree_node {
  int sum[3]; // suma lui 4^i pentru pozițiile i pe care valoarea este 0/1/2
  int sum_all; // suma puterilor lui 4 subîntinse
  unsigned char lazy_val;
  unsigned char lazy_incs; // modulo 3

  bool is_lazy() {
    return (lazy_val != NONE) || lazy_incs;
  }

  // Atribuie tuturor frunzelor subîntinse valoarea val.
  void set(int val) {
    sum[0] = sum[1] = sum[2] = 0;
    sum[val] = sum_all;
    lazy_val = val;
    lazy_incs = 0;
  }

  // Valoarea lui f[] pe această poziție este indicată de cantitatea nenulă
  // din sum[].
  int get() {
    return sum[0] ? 0 : (sum[1] ? 1 : 2);
  }

  // Crește toate frunzele subîntinse cu val (modulo 3).
  void inc(int val) {
    if (lazy_val != NONE) {
      set(mod3[lazy_val + val]);
    } else {
      rotate(val);
      lazy_incs = mod3[lazy_incs + val];
    }
  }

  void rotate(int cnt) {
    if (cnt == 1) {
      int tmp = sum[2];
      sum[2] = sum[1];
      sum[1] = sum[0];
      sum[0] = tmp;
    } else if (cnt == 2) {
      int tmp = sum[0];
      sum[0] = sum[1];
      sum[1] = sum[2];
      sum[2] = tmp;
    }
  }

  void push(segment_tree_node& a, segment_tree_node& b) {
    if (lazy_val != NONE) {
      a.set(lazy_val);
      b.set(lazy_val);
      lazy_val = NONE;
    } else if (lazy_incs) {
      a.inc(lazy_incs);
      b.inc(lazy_incs);
      lazy_incs = 0;
    }
  }

  void pull(segment_tree_node& a, segment_tree_node& b) {
    for (int i = 0; i < 3; i++) {
      sum[i] = (a.sum[i] + b.sum[i]) % MOD;
    }
  }
};

struct segment_tree {
  segment_tree_node v[MAX_POW_2]; // fără dublare, căci are mărime n/2
  int n, bits;

  void init(int _n) {
    n = next_power_of_2(_n);
    bits = __builtin_popcount(n - 1);
  }

  void raw_set(int pos, int val) {
    int p = pos + n;
    v[p].sum_all = pow4[pos];
    v[p].set(val);
  }

  void build() {
    for (int i = n - 1; i; i--) {
      v[i].pull(v[2 * i], v[2 * i + 1]);
      // sum_all nu se schimbă după construcție
      v[i].sum_all = (v[2 * i].sum_all + v[2 * i + 1].sum_all) % MOD;
      v[i].lazy_val = NONE;
      v[i].lazy_incs = 0;
    }
  }

  void push_path(int pos) {
    for (int b = bits; b; b--) {
      int ancestor = pos >> b;
      v[ancestor].push(v[2 * ancestor], v[2 * ancestor + 1]);
    }
  }

  void pull_path(int pos) {
    for (pos /= 2; pos; pos /= 2) {
      if (!v[pos].is_lazy()) {
        v[pos].pull(v[2 * pos], v[2 * pos + 1]);
      }
    }
  }

  void range_set(int l, int r, int val) {
    l += n;
    r += n;
    int orig_l = l, orig_r = r;

    push_path(l);
    push_path(r);

    while (l <= r)  {
      if (l & 1) {
        v[l++].set(val);
      }
      l >>= 1;

      if (!(r & 1)) {
        v[r--].set(val);
      }
      r >>= 1;
    }

    pull_path(orig_l);
    pull_path(orig_r);
  }

  void prefix_inc(int r, int val) {
    r += n;
    int orig_r = r;

    push_path(r);

    // ultimul nod acoperit de [0,r] va avea indicele putere a lui 2.
    while (r & (r - 1)) {
      if (!(r & 1)) {
        v[r--].inc(val);
      }
      r >>= 1;
    }
    v[r].inc(val);

    pull_path(orig_r);
  }

  int get(int pos) {
    pos += n;
    push_path(pos);
    return v[pos].get();
  }
};

r_segment_tree r;
segment_tree f[2];
FILE *fin, *fout;
int n, num_ops;

void read_disks() {
  fscanf(fin, "%d %d", &n, &num_ops);
  r.init(n);

  for (int i = 0; i < 3; i++) {
    int num_disks, size;
    fscanf(fin, "%d", &num_disks);
    while (num_disks--) {
      fscanf(fin, "%d", &size);
      size--;
      r.set(size, i);
    }
  }
}

void compute_pow4() {
  pow4[0] = 1;
  for (int i = 1; i <= n / 2; i++) {
    pow4[i] = 4 * pow4[i - 1] % MOD;
  }
}

void init_f() {
  f[0].init((n + 1) / 2);
  f[1].init(n / 2);

  int expected = r.get(n - 1);
  for (int i = n - 1; i >= 0; i--) {
    int rod = r.get(i);
    int diff = mod3[3 + rod - expected];
    f[i % 2].raw_set(i / 2, diff);

    if (rod != expected) {
      expected = 3 - rod - expected;
    }
  }

  f[0].build();
  f[1].build();
}

int compute_cost() {
  // suma pe pozițiile unde f ≠ 0, adică f ∊ { 1, 2}
  return (f[0].v[1].sum[1] +
          f[0].v[1].sum[2] +
          2 * f[1].v[1].sum[1] +
          2 * f[1].v[1].sum[2]) % MOD;
}

// Returnează noul f[pos] cînd r[pos] se schimbă.
int get_new_f(int pos, int new_r) {
  if (pos == n - 1) {
    return 0; // Discul n-1 este întotdeauna pe tija corectă.
  } else {
    int old_r = r.get(pos);
    int old_f = f[pos % 2].get(pos / 2);
    int e = mod3[3 + old_r - old_f];
    return mod3[3 + new_r - e];
  }
}

// Returnează variația lui f[pos-1] cînd r[pos] se schimbă.
int get_delta_f(int pos, int new_r) {
  if (!pos) {
    return 0;
  }

  int f_pos = f[pos % 2].get(pos / 2);
  int r_pos = new_r;
  int e_pos = mod3[3 + r_pos - f_pos];

  int f_prev = f[(pos - 1) % 2].get((pos - 1) / 2);
  int r_prev = r.get(pos - 1);
  int e_prev = mod3[3 + r_prev - f_prev];

  int new_e = (r_pos == e_pos) ? e_pos : (3 - e_pos - r_pos);

  if (new_e == e_prev) {
    return 0;
  } else {
    int new_f = mod3[3 + r_prev - new_e];
    return mod3[3 + new_f - f_prev];
  }
}

void update_rod(int from, int to, int new_rod) {
  // Calculăm cele două valori pentru alternanța pe pozițiile from...to.
  // Un pic de magie ca să calculăm intervalele din f[0] și în f[1].
  int new_f_to = get_new_f(to, new_rod);

  f[to % 2].range_set((from + 1 - to % 2) / 2, to / 2, new_f_to);
  if (from < to) {
    int other_f = mod3[3 - new_f_to];
    f[1 - to % 2].range_set((from + to % 2) / 2, (to - 1) / 2, other_f);
  }

  // Corectăm vectorul r.
  r.range_set(from, to, new_rod);

  // Ne uităm dacă e[from-1] s-a modificat. Dacă nu, așteptările pentru
  // discurile 1...from-1 nu se modifică. Dacă da, ele variază cu ±1.
  int delta = get_delta_f(from, new_rod);
  if (delta) {
    f[(from - 1) % 2].prefix_inc((from - 1) / 2, delta);
    if (from >= 2) {
      f[1 - (from - 1) % 2].prefix_inc(from / 2 - 1, 3 - delta);
    }
  }
}

void process_ops() {
  fprintf(fout, "%d\n", compute_cost());

  while (num_ops--) {
    int from, to, new_rod;
    fscanf(fin, "%d %d %d", &from, &to, &new_rod);
    from--;
    new_rod--;
    to--;
    update_rod(from, to, new_rod);
    fprintf(fout, "%d\n", compute_cost());
  }
}

int main() {
  fin = fopen("babel.in", "r");
  fout = fopen("babel.out", "w");

  read_disks();
  compute_pow4();
  init_f();
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
