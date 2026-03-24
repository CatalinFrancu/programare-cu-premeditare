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

  // Rotește t spre dreapta și returnează noua rădăcină.
  int rotate_right(int t) {
    int x = v[t].l;
    v[t].l = v[x].r;
    v[x].r = t;
    update_cnt(t);
    update_cnt(x);
    return x;
  }

  // Rotește t spre stînga și returnează noua rădăcină.
  int rotate_left(int t) {
    int x = v[t].r;
    v[t].r = v[x].l;
    v[x].l = t;
    update_cnt(t);
    update_cnt(x);
    return x;
  }

  // Inserează cheia key în subarborele lui t și scrie în t noua rădăcină.
  void insert(int& t, int key) {
    if (!t) {
      v[n] = { .key = key, .pri = rand(), .cnt = 1, .l = 0, .r = 0 };
      t = n++;
    } else if (key < v[t].key) {
      // Inserează în stînga. Contăm pe fiul stîng să se reorganizeze intern.
      // Rotește spre dreapta dacă t și fiul stîng au prioritățile inversate.
      insert(v[t].l, key);
      if (v[v[t].l].pri > v[t].pri) {
        t = rotate_right(t);
      }
    } else {
      insert(v[t].r, key);
      if (v[v[t].r].pri > v[t].pri) {
        t = rotate_left(t);
      }
    }
    update_cnt(t);
  }

  void insert(int key) {
    insert(root, key);
  }

  // Șterge cheia key din subarborele lui t și scrie în t noua rădăcină. Cheia
  // poate să nu existe în treap.
  void erase(int& t, int key) {
    if (!t) {
      return; // Nimic de făcut, cheia nu există în treap.
    } else if (key < v[t].key) {
      erase(v[t].l, key);
    } else if (key > v[t].key) {
      erase(v[t].r, key);
    } else if (!v[t].l && !v[t].r) {
      t = 0; // Am împins cheia pînă într-o frunză.
    } else if (!v[t].l || (v[t].r && (v[v[t].r].pri > v[v[t].l].pri))) {
      // Împinge elementul de șters cu un nivel mai jos printr-o rotație la
      // stînga sau la dreapta, în funcție de prioritățile fiilor.
      t = rotate_left(t);
      erase(v[t].l, key);
    } else {
      t = rotate_right(t);
      erase(v[t].r, key);
    }
    update_cnt(t);
  }

  void erase(int key) {
    erase(root, key);
  }
};
