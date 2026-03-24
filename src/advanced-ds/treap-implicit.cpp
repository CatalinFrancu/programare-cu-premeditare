// Treap implicit implementat cu split și merge.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

const int N = 1'000;
const int MAX_VAL = 1'000'000;

struct node {
  int val, pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[10 * N + 1]; // Nu facem garbage collection.
  int n, root;

  void init() {
    n = 1; // Marchează nodul 0 ca folosit, întrucît 0 înseamnă NULL.
    root = 0;
  }

  void update_cnt(int t) {
    if (t) {
      v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
    }
  }

  // Sparge subarborele lui t în primele pos valori și restul și pune cele
  // două rădăcini în l și în r.
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

  // Unifică subarborii lui l și r și pune rădăcina rezultată în t.
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

  // Inserează elementul elem la poziția pos și scrie în t noua rădăcină.
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

  void insert(int pos, int val) {
    v[n] = { .val = val, .pri = rand(), .cnt = 1, .l = 0, .r = 0 };
    insert(root, pos, n++);
  }

  // Șterge poziția pos din subarborele lui t și scrie în t noua rădăcină.
  void erase(int& t, int pos) {
    if (pos == v[v[t].l].cnt) {
      merge(t, v[t].l, v[t].r);
    } else if (pos < v[v[t].l].cnt) {
      erase(v[t].l, pos);
    } else {
      erase(v[t].r, pos - 1 - v[v[t].l].cnt);
    }
    update_cnt(t);
  }

  void erase(int pos) {
    erase(root, pos);
  }

  // Returnează al pos-lea element (practic o rescriere a lui kth_element).
  int get_ptr(int pos) {
    int t = root;

    while (pos != v[v[t].l].cnt) {
      if (pos < v[v[t].l].cnt) {
        t = v[t].l;
      } else {
        pos -= v[v[t].l].cnt + 1;
        t = v[t].r;
      }
    }

    return t;
  }

  int get(int pos) {
    int t = get_ptr(pos);
    return v[t].val;
  }

  void set(int pos, int val) {
    int t = get_ptr(pos);
    v[t].val = val;
  }
};

treap t;
std::vector<int> v;
int values[N];

void compare() {
  for (unsigned i = 0; i < v.size(); i++) {
    assert(v[i] == t.get(i));
  }
}

void insert_op() {
  if (v.size() < N) {
    int pos = rand() % (v.size() + 1);
    int val = rand() % (MAX_VAL + 1);
    v.insert(v.begin() + pos, val);
    t.insert(pos, val);
    compare();
  }
}

void erase_op() {
  if (v.size()) {
    int pos = rand() % v.size();
    v.erase(v.begin() + pos);
    t.erase(pos);
    compare();
  }
}

void set_op() {
  if (v.size()) {
    int pos = rand() % v.size();
    int val = rand() % (MAX_VAL + 1);
    v[pos] = val;
    t.set(pos, val);
    compare();
  }
}

int main() {
  srand(time(NULL));

  t.init();

  // Umple-l pînă la 50%.
  for (int i = 0; i < N / 2; i++) {
    insert_op();
  }

  // Fă diverse operații.
  for (int i = 0; i < N * 10; i++) {
    switch (rand() % 3) {
      case 0: insert_op(); break;
      case 1: erase_op(); break;
      case 2: set_op(); break;
    }
  }

  // Golește-l.
  while (v.size()) {
    erase_op();
  }

  return 0;
}
