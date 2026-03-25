#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 200'000;

struct node {
  char c;
  int pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[MAX_N + 1];
  int root;

  void update_cnt(int t) {
    if (t) {
      v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
    }
  }

  void split(int t, int pos, int& l, int& r) {
    if (!t) {
      l = r = 0;
    } else if (pos <= v[v[t].l].cnt) {
      split(v[t].l, pos, l, v[t].l);
      r = t;
      update_cnt(t);
    } else {
      split(v[t].r, pos - v[v[t].l].cnt - 1, v[t].r, r);
      l = t;
      update_cnt(t);
    }
  }

  void merge(int& t, int l, int r) {
    if (!l) {
      t = r;
    } else if (!r) {
      t = l;
    } else if (v[l].pri > v[r].pri) {
      merge(v[l].r, v[l].r, r);
      t = l;
    } else {
      merge(v[r].l, l, v[r].l);
      t = r;
    }
    update_cnt(t);
  }

  void insert(int& t, int pos, int elem) {
    if (!t) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, pos, v[elem].l, v[elem].r);
      t = elem;
    } else if (pos <= v[v[t].l].cnt) {
      insert(v[t].l, pos, elem);
    } else {
      insert(v[t].r, pos - v[v[t].l].cnt - 1, elem);
    }
    update_cnt(t);
  }

  void move_to_end(int a, int b) {
    int x, y, z;
    split(root, a, x, y);
    split(y, b - a + 1, y, z);
    merge(x, x, z);
    merge(root, x, y);
  }

  void read(int len) {
    root = 0;
    for (int i = 1; i <= len; i++) {
      v[i] = { .c = (char)getchar(), .pri = rand(), .cnt = 1, .l = 0, .r = 0 };
      insert(root, i, i);
    }
  }

  void write(int t) {
    if (t) {
      write(v[t].l);
      putchar(v[t].c);
      write(v[t].r);
    }
  }

  void write() {
    write(root);
    putchar('\n');
  }
};

treap t;

int main() {
  int n, q, a, b;
  scanf("%d %d ", &n, &q);
  t.read(n);

  while (q--) {
    scanf("%d %d", &a, &b);
    t.move_to_end(a - 1, b - 1);
  }

  t.write();

  return 0;
}
