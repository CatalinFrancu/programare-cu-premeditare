struct node {
  int key, pri;
  int cnt;
  int l, r;
};

struct treap {
  node v[N + 1];
  int n, root;

  void init() {
    n = 1; // Marchează nodul 0 ca folosit, întrucît 0 înseamnă NULL.
    root = 0;
  }

  int search(int key) {
    int t = root;
    while (t && (key != v[t].key)) {
      t = (key < v[t].key) ? v[t].l : v[t].r;
    }
    return t;
  }

  bool contains(int key) {
    return search(key) != 0;
  }

  int order_of(int key) {
    int result = 0;
    int t = root;
    while (t) {
      if (key > v[t].key) {
        result += 1 + v[v[t].l].cnt;
        t = v[t].r;
      } else {
        t = v[t].l;
      }
    }

    return result;
  }

  int kth_element(int k) {
    int t = root;
    while (k != v[v[t].l].cnt) {
      if (k < v[v[t].l].cnt) {
        t = v[t].l;
      } else {
        k = k - 1 - v[v[t].l].cnt;
        t = v[t].r;
      }
    }
    return v[t].key;
  }

  void update_cnt(int t) {
    if (t) {
      v[t].cnt = 1 + v[v[t].l].cnt + v[v[t].r].cnt;
    }
  }

  // Sparge subarborele lui t în valori ≤ key și > key și pune cele două
  // rădăcini în l și în r.
  void split(int t, int key, int& l, int& r) {
    if (!t) {
      l = r = 0;
    } else if (v[t].key <= key) {
      split(v[t].r, key, v[t].r, r);
      l = t;
      update_cnt(t);
    } else {
      split(v[t].l, key, l, v[t].l);
      r = t;
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

  // Inserează elementul în subarborele lui t și scrie în t noua rădăcină.
  void insert(int& t, int elem) {
    if (!t) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].key, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(v[t].key <= v[elem].key ? v[t].r : v[t].l, elem);
    }
    update_cnt(t);
  }

  void insert(int key) {
    v[n] = { .key = key, .pri = rand(), .cnt = 1, .l = 0, .r = 0 };
    insert(root, n++);
  }

  // Șterge cheia key din subarborele lui t și scrie în t noua rădăcină.
  void erase(int& t, int key) {
    if (v[t].key == key) {
      merge(t, v[t].l, v[t].r);
    } else {
      erase(key < v[t].key ? v[t].l : v[t].r, key);
    }
    update_cnt(t);
  }

  void erase(int key) {
    erase(root, key);
  }
};
