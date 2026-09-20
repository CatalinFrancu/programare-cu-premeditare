// Complexitate: O(n log n).
#include <algorithm>
#include <stdio.h>

typedef long long i64;

const int MAX_POINTS = 200'000;
const int MAX_P2 = 1 << 18;
const i64 INF = 1e18;

struct point {
  int x, y, val;
};

point p[MAX_POINTS];
// Greutățile totale pentru fiecare y la stînga și la dreapta x-ului curent.
i64 real_l[MAX_POINTS + 1], real_r[MAX_POINTS + 1];
int n;

i64 min(i64 x, i64 y) {
  return (x < y) ? x : y;
}

i64 max(i64 x, i64 y) {
  return (x > y) ? x : y;
}

i64 min4(i64 a, i64 b, i64 c, i64 d) {
  return min(min(a, b), min(c, d));
}

i64 max4(i64 a, i64 b, i64 c, i64 d) {
  return max(max(a, b), max(c, d));
}

// Stochează sumele celor patru cadrane pentru o coordonată y dată, pentru
// x-ul curent.
struct split {
  i64 tl, bl, tr, br; // top-left, bottom-left, top-right, bottom-right
  int y;

  void advance(i64 dl, i64 dr, int delta_y) {
    tl -= dl;
    bl += dl;
    tr -= dr;
    br += dr;
    y += delta_y;
  }

  void advance() {
    y++;
    tl -= real_l[y];
    bl += real_l[y];
    tr -= real_r[y];
    br += real_r[y];
  }

  i64 get_cost() {
    return max4(tl, tr, bl, br) - min4(tl, tr, bl, br);
  }
};

struct fenwick_tree {
  i64 l[MAX_POINTS + 1], r[MAX_POINTS + 1], total_l, total_r;
  int n, max_p2;

  void init(int n) {
    this->n = max_p2 = n;

    while (max_p2 & (max_p2 - 1)) {
      max_p2 &= max_p2 - 1;
    }
  }

  void add(i64* v, int pos, int val) {
    do {
      v[pos] += val;
      pos += pos & -pos;
    } while (pos <= n);
  }

  void add_right(int pos, int val) {
    add(r, pos, val);
    total_r += val;
  }

  void move_left(int pos, int val) {
    add(l, pos, val);
    total_l += val;
    add(r, pos, -val);
    total_r -= val;
  }

  // Căutare binară pentru o funcție treaptă.
  split find_overtake_pos(bool min_ascend) {
    split s = {
      .tl = total_l, .bl = 0,
      .tr = total_r, .br = 0,
      .y = 0,
    };

    for (int interval = max_p2; interval; interval >>= 1) {
      if (s.y + interval <= n) {
        i64 dl = l[s.y + interval], dr = r[s.y + interval];
        bool cond =
          (min_ascend && (min(s.bl + dl, s.br + dr) < min(s.tl - dl, s.tr - dr))) ||
          (!min_ascend && (max(s.tl - dl, s.tr - dr) > max(s.bl + dl, s.br + dr)));
        if (cond) {
          s.advance(dl, dr, interval);
        }
      }
    }

    return s;
  }

  // Returnează cel mai mare y a.î. minimul este bl sau br.
  split find_min_ascend_pos() {
    return find_overtake_pos(true);
  }

  // Returnează cel mai mare y a.î. maximul este tl sau tr.
  split find_max_descend_pos() {
    return find_overtake_pos(false);
  }
};

// Un arbore de intervale cu suport pentru operațiile:
//
// 1. Add(L, R, X): adaugă X la fiecare poziție din [L, R].
// 2. Min(L, R): returnează valoarea minimă din [L, R].
// 3. Max(L, R): returnează valoarea maximă din [L, R].
//
// Contract: lazy trebuie adăugat la mi și la ma pe întregul subarbore.
struct segment_tree {
  i64 mi[2 * MAX_P2], ma[2 * MAX_P2]; // I love my Meemaw.
  i64 lazy[2 * MAX_P2];
  int n;

  void init(int n) {
    while (n & (n - 1)) {
      n += (n & -n);
    }
    this->n = n;
  }

  void push(int node) {
    mi[node] += lazy[node];
    ma[node] += lazy[node];
    lazy[2 * node] += lazy[node];
    lazy[2 * node + 1] += lazy[node];
    lazy[node] = 0;
  }

  void pull(int node) {
    mi[node] = min(mi[2 * node] + lazy[2 * node],
                   mi[2 * node + 1] + lazy[2 * node + 1]);
    ma[node] = max(ma[2 * node] + lazy[2 * node],
                   ma[2 * node + 1] + lazy[2 * node + 1]);
  }

  // [L, R)
  void update(int node, int pl, int pr, int l, int r, int delta) {
    if (l >= r) {
      return;
    } else if ((l == pl) && (r == pr)) {
      lazy[node] += delta;
    } else {
      push(node);
      int mid = (pl + pr) >> 1;
      update(2 * node, pl, mid, l, min(r, mid), delta);
      update(2 * node + 1, mid, pr, max(l, mid), r, delta);
      pull(node);
    }
  }

  // [L, R]
  void update(int l, int r, int delta) {
    update(1, 0, n, l, r + 1, delta);
  }

  i64 query(int node, int pl, int pr, int l, int r, bool is_min) {
    if (l >= r) {
      return is_min ? INF : -INF;
    } else if ((l == pl) && (r == pr)) {
      return lazy[node] + (is_min ? mi[node] : ma[node]);
    } else {
      push(node);
      int mid = (pl + pr) >> 1;
      i64 left = query(2 * node, pl, mid, l, min(r, mid), is_min);
      i64 right = query(2 * node + 1, mid, pr, max(l, mid), r, is_min);
      return is_min ? min(left, right) : max(left, right);
    }
  }

  i64 min_query(int l, int r) {
    return query(1, 0, n, l, r + 1, true);
  }

  i64 max_query(int l, int r) {
    return query(1, 0, n, l, r + 1, false);
  }
};

fenwick_tree fen;
segment_tree st_top, st_bottom;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d", &p[i].x, &p[i].y, &p[i].val);
  }
}

void sort_by_x() {
  std::sort(p, p + n, [] (point a, point b) {
    return a.x < b.x;
  });
}

void find_and_print_best() {
  split s1 = fen.find_min_ascend_pos();
  split s2 = fen.find_max_descend_pos();
  i64 cost;

  if (s1.y < s2.y) {
    // Ambele extreme sus.
    s2.advance();
    cost = min(s1.get_cost(), s2.get_cost());
    s1.advance();
    if (s1.tl < s1.tr) {
      // TL este min, TR este max
      cost = min(cost, -st_top.max_query(s1.y, s2.y - 1));
    } else {
      // TL este max, TR este min
      cost = min(cost, st_top.min_query(s1.y, s2.y - 1));
    }
  } else {
    // Ambele extreme jos.
    s1.advance();
    cost = min(s2.get_cost(), s1.get_cost());
    s2.advance();
    if (s2.bl < s2.br) {
      // BL este min, BR este max
      cost = min(cost, -st_bottom.max_query(s2.y, s1.y - 1));
    } else {
      // BL este max, BR este min
      cost = min(cost, st_bottom.min_query(s2.y, s1.y - 1));
    }
  }

  printf("%lld\n", cost);
}

void scan_points() {
  // TODO: Le putem construi în O(n).
  fen.init(n);
  st_top.init(n + 1);
  st_bottom.init(n + 1);
  for (int i = 0; i < n; i++) {
    fen.add_right(p[i].y, p[i].val);
    real_r[p[i].y] += p[i].val;
    st_top.update(0, p[i].y - 1, -p[i].val);
    st_bottom.update(p[i].y, n, -p[i].val);
  }

  int i = 0;
  for (int x = 1; x < n; x++) {
    while ((i < n) && (p[i].x <= x)) {
      fen.move_left(p[i].y, p[i].val);
      real_r[p[i].y] -= p[i].val;
      real_l[p[i].y] += p[i].val;
      st_top.update(0, p[i].y - 1, 2 * p[i].val);
      st_bottom.update(p[i].y, n, 2 * p[i].val);
      i++;
    }
    find_and_print_best();
  }
}

int main() {
  read_data();
  sort_by_x();
  scan_points();

  return 0;
}
