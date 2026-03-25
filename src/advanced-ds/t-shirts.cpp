#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_TSHIRTS = 200'000;
const int MAX_CUSTOMERS = 200'000;

int answer[MAX_CUSTOMERS];

struct tshirt {
  int cost;
  int quality;
};

// Contract:
//
// Valoarea reală a unui nod este bugetul minus suma valorilor lazy_budget din
// toți strămoșii, inclusiv nodul însuși. Numărul de tricouri cumpărate de un
// nod este suma valorilor lazy_bought din toți strămoșii inclusiv el însuși.
struct node {
  int pri;
  int budget, bought;
  int lazy_budget, lazy_bought;
  int orig_id;
  int l, r;
};

struct treap {
  node v[MAX_CUSTOMERS + 1];
  int n, root;

  node make_node(int budget, int orig_id) {
    int r = ((rand() & 0x7fff) << 15) | (rand() & 0x7fff);
    return {
      .pri = r,
      .budget = budget,
      .bought = 0,
      .lazy_budget = 0,
      .lazy_bought = 0,
      .orig_id = orig_id,
      .l = 0,
      .r = 0,
    };
  }

  void init() {
    n = 1;
    root = 0;
  }

  void push(int t) {
    if (v[t].l) {
      v[v[t].l].lazy_budget += v[t].lazy_budget;
      v[v[t].l].lazy_bought += v[t].lazy_bought;
    }
    if (v[t].r) {
      v[v[t].r].lazy_budget += v[t].lazy_budget;
      v[v[t].r].lazy_bought += v[t].lazy_bought;
    }
    v[t].budget += v[t].lazy_budget;
    v[t].bought += v[t].lazy_bought;
    v[t].lazy_budget = 0;
    v[t].lazy_bought = 0;
  }

  void split(int t, int budget, int& l, int& r) {
    if (!t) {
      l = r = 0;
      return;
    }

    push(t);
    if (v[t].budget < budget) {
      split(v[t].r, budget, v[t].r, r);
      l = t;
    } else {
      split(v[t].l, budget, l, v[t].l);
      r = t;
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
  }

  void _insert(int& t, int elem) {
    if (!t) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].budget, v[elem].l, v[elem].r);
      t = elem;
    } else {
      push(t);
      _insert(v[elem].budget <= v[t].budget ? v[t].l : v[t].r, elem);
      // Vezi https://codeforces.com/blog/entry/92340#comment-814503 pentru
      // motivul operatorului <=.
    }
  }

  void insert(int budget, int orig_id) {
    v[n] = make_node(budget, orig_id);
    _insert(root, n++);
  }

  void insert_all(int& dest, int src) {
    if (src) {
      push(src);
      insert_all(dest, v[src].l);
      insert_all(dest, v[src].r);
      v[src].l = v[src].r = 0; // decuplează nodul de treapul prezent
      _insert(dest, src);
    }
  }

  void purchase(int cost) {
    int no_buy, buy, poor, rich;
    split(root, cost, no_buy, buy);
    v[buy].lazy_bought++;
    v[buy].lazy_budget -= cost;
    split(buy, cost, poor, rich);
    insert_all(no_buy, poor);
    merge(root, no_buy, rich);
  }

  void _collect_answers(int t) {
    if (t) {
      push(t);
      answer[v[t].orig_id] = v[t].bought;
      _collect_answers(v[t].l);
      _collect_answers(v[t].r);
    }
  }

  void collect_answers() {
    _collect_answers(root);
  }
};

treap t;
tshirt tee[MAX_TSHIRTS];
int num_tshirts, num_customers;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

void read_data() {
  scanf("%d", &num_tshirts);
  for (int i = 0; i < num_tshirts; i++) {
    scanf("%d %d", &tee[i].cost, &tee[i].quality);
  }

  t.init();
  scanf("%d", &num_customers);
  for (int i = 0; i < num_customers; i++) {
    int budget;
    scanf("%d", &budget);
    t.insert(budget, i);
  }
}

void sort_tshirts() {
  std::sort(tee, tee + num_tshirts, [](tshirt a, tshirt b) {
    return (a.quality > b.quality) ||
      ((a.quality == b.quality) && (a.cost < b.cost));
  });
}

void iterate_thirts() {
  for (int i = 0; i < num_tshirts; i++) {
    t.purchase(tee[i].cost);
  }
}

void write_answers() {
  for (int i = 0; i < num_customers; i++) {
    printf("%d ", answer[i]);
  }
  printf("\n");
}

int main() {
  init_rng();
  read_data();
  sort_tshirts();
  iterate_thirts();
  t.collect_answers();
  write_answers();

  return 0;
}
