#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 200'000;
const int T_REVERSE = 1;

// Contract:
//
// În orice nod u, subtree_sum este suma subarborelui cu rădăcina în u. Ea
// este menținută în timp real, ca și cnt (folosit pentru treapul implicit).
// Cîmpul boolean flip indică că subarborele curent trebuie răsturnat și este
// menținut lazy. Un subarbore trebuie realmente răsturnat dacă xor-ul biților
// flip din toți strămoșii, inclusiv el însuși, este 1.
struct node {
  int val, pri;
  int cnt;
  bool flip;
  long long subtree_sum;
  int l, r;
};

struct treap {
  node v[MAX_N + 1];
  int root;

  void update_node(int t) {
    if (t) {
      v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
      v[t].subtree_sum = v[t].val + v[v[t].l].subtree_sum + v[v[t].r].subtree_sum;
    }
  }

  void push(int t) {
    if (v[t].flip) {
      v[t].flip = false;
      if (v[t].l) {
        v[v[t].l].flip ^= 1;
      }
      if (v[t].r) {
        v[v[t].r].flip ^= 1;
      }
      int aux = v[t].l;
      v[t].l = v[t].r;
      v[t].r = aux;
    }
  }

  void split(int t, int pos, int& l, int& r) {
    if (!t) {
      l = r = 0;
      return;
    }

    // Cînd coborîm pe fiul stîng sau drept, este important să știm care este
    // care.
    push(t);
    if (pos <= v[v[t].l].cnt) {
      split(v[t].l, pos, l, v[t].l);
      r = t;
    } else {
      split(v[t].r, pos - v[v[t].l].cnt - 1, v[t].r, r);
      l = t;
    }
    update_node(t);
  }

  void merge(int& t, int l, int r) {
    if (!l) {
      t = r;
    } else if (!r) {
      t = l;
    } else if (v[l].pri > v[r].pri) {
      // Nodul l devine rădăcină. În fiul drept al lui l vom unifica fiul său
      // drept curent cu nodul r. Deci este important ca l să știe foarte bine
      // cine este fiul său drept.
      push(l);
      merge(v[l].r, v[l].r, r);
      t = l;
    } else {
      push(r);
      merge(v[r].l, l, v[r].l);
      t = r;
    }
    update_node(t);
  }

  // Prin natura problemei, aici nu este important să facem push, deoarece
  // încă nu avem informații lazy.
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
    update_node(t);
  }

  void reverse(int a, int b) {
    int x, y, z;
    split(root, a, x, y);
    split(y, b - a + 1, y, z);
    v[y].flip = true;
    merge(x, x, y);
    merge(root, x, z);
  }

  long long range_sum(int a, int b) {
    int x, y, z;
    split(root, a, x, y);
    split(y, b - a + 1, y, z);
    // Rezultatul nu este influențat de biții flip din subarbore.
    long long result = v[y].subtree_sum;
    merge(x, x, y);
    merge(root, x, z);
    return result;
  }

  void read(int n) {
    root = 0;
    for (int i = 1; i <= n; i++) {
      int val;
      scanf("%d", &val);
      v[i] = { .val = val, .pri = rand(), .l = 0, .r = 0 };
      insert(root, i, i);
    }
  }
};

treap t;

int main() {
  int n, num_ops;
  scanf("%d %d", &n, &num_ops);
  t.read(n);

  while (num_ops--) {
    int type, a, b;
    scanf("%d %d %d", &type, &a, &b);
    a--;
    b--;
    if (type == T_REVERSE) {
      t.reverse(a, b);
    } else {
      printf("%lld\n", t.range_sum(a, b));
    }
  }

  return 0;
}
